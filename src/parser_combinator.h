#pragma once
#include <string>
#include <istream>
#include <functional>
#include <memory>

// algebraic data structures
///////////////////////////////////////////////////////////////////////////////

namespace parser::alg
{
  /*! Product type of two parameters */
  template <typename T1, typename T2>
  class Both
  {
  public:
    T1 lx; //!< left value
    T2 rx; //!< right value
    /*!
     * Constructor for both values
     * @param _lx left value
     * @param _rx right value
     */
    Both(T1 _lx, T2 _rx) : lx(_lx), rx(_rx) {}
  };

  /*! Disjoint sum type of two parameters */
  template <typename T1, typename T2>
  class Either
  {
  public:
    bool left; //!< flag to indicate object contains left value
    T1 lx;     //!< left value
    T2 rx;     //!< right value
    /*!
     * Constructor for left value
     * @param x left value
     */
    template <typename T1_, typename T2_>
    friend Either<T1_, T2_> Left(T1_ *x);
    /*!
     * Constructor for right value
     * @param x right value
     */
    template <typename T1_, typename T2_>
    friend Either<T1_, T2_> Right(T2_ *x);
  };

  template <typename T1_, typename T2_>
  Either<T1_, T2_> Left(T1_ x)
  {
    Either<T1_, T2_> e;
    e.left = true;
    e.lx = x;
    return e;
  }

  template <typename T1_, typename T2_>
  Either<T1_, T2_> Right(T2_ x)
  {
    Either<T1_, T2_> e;
    e.left = false;
    e.rx = x;
    return e;
  }
}

// algebraic util functions
///////////////////////////////////////////////////////////////////////////////

namespace parser::alg::util
{
  /*! Returns first element of a pair */
  template <typename T, typename U>
  T fst(const Both<T, U> &b) { return b.lx; }
  /*! Returns second element of a pair */
  template <typename T, typename U>
  U snd(const Both<T, U> &b) { return b.rx; }
  /*!
   * Given a three element Both, return its middle element
   * @param b three element
   * @return middle value
   */
  template <typename T, typename U, typename V>
  U get_mid(const Both<Both<T, U>, V> &b)
  {
    return b.lx.rx;
  }
  template <typename T, typename U, typename V>
  U get_mid(const Both<T, Both<U, V>> &b)
  {
    return b.rx.lx;
  }
  /*!
   * Given an either where both sides are the same type,
   * return either which value
   * @param e either object
   * @return either value
   */
  template <typename T>
  T get_either(const Either<T, T> &e)
  {
    return e.left ? e.lx : e.rx;
  }
}

// parser state classes
///////////////////////////////////////////////////////////////////////////////

namespace parser::state {
  const std::string STATE_NOT_FAILED_LABEL = "<not failed>"; //!< label when state not in failure state
  
  struct empty {}; //!< empty struct for default user data in state

  /*! State object virtual class */
  template <typename X = empty>
  class State {
  protected:
    int i;                      //!< index to currently to be consumed character
  private:
    bool failed;                //!< flag to determine failure
    int fail_uuid;              //!< uuid of failing parser
  public:
    X data;                     //!< user data
    /*!
     * Construct state with pointer to source string
     */
    State() : i(0), failed(false) {}
    /*!
     * Advance the currently consumed character (must be implemented by inheriting class)
     * @return consumed character
     */
    virtual const char adv() {
      throw std::vector<State<X>>();
    }
    /*!
     * Set state to failure with parser uuid
     * @param id failure uuid
     */
    void fail(const int id) {
      if(!failed) {
        failed = true;
        fail_uuid = id;
      }
    }
    /*!
     * Get failure flag
     * @return flag
     */
    const bool has_failed() { return failed; }
    /*!
     * Get failure parser uuid
     * @return failure id
     */
    const int get_fail() { return failed ? fail_uuid : -1; }
  };

  /*! State using std::string as source */
  template <typename X = empty>
  class StateString : public State<X> {
  private:
    std::string *src;           //!< pointer to source string to be parsed
  public:
    StateString(std::string *_src) : State<X>(), src(_src) {}
    const char adv() override {
      if (this->i >= src->size()) throw std::vector<State<X>>();
      return src->at(this->i++);
    }
  };

  /*! State using std::istream as source */
  template <typename X = empty>
  class StateIStream : public State<X> {
  private:
    std::istream *src;          //!< pointer to input stream used to parse
  public:
    StateIStream(std::istream *_src) : State<X>(), src(_src) {}
    const char adv() override {
      char c;
      if (this->i != src->tellg()) src->seekg(this->i);
      *src >> c;
      if (c == 0) throw std::vector<State<X>>();
      this->i++;
      return c;
    }
  };
}

// parser class
///////////////////////////////////////////////////////////////////////////////

namespace parser {
  using namespace parser::state;

  enum parser_type { USER, SEQ, ALT, MAP, MANY, SOME }; //!< parser type values
  const std::string label_str[] = { "seq", "alt", "map", "many", "some"}; //!< mapping parser type to string

  /*! Class for parser metadata inclusive of unique id and children parser metadata */
  class ParserMetaData {
  public:
    const int uuid;
    const std::string user_label;
    const parser_type type;
    const std::vector<ParserMetaData const*> children;
    ParserMetaData(std::string _label, std::vector<ParserMetaData const*> _children = {}) : uuid(__COUNTER__), user_label(_label), type(USER), children(_children) {}
    ParserMetaData(parser_type _type, std::vector<ParserMetaData const*> _children = {}) : uuid(__COUNTER__), type(_type), children(_children) {}
  };

  /*! Parser base class */
  template <typename T, typename X = empty>
  class Parser {
  private:
    const std::function<T(State<X> &)> f; //!< core function
  public:
    const ParserMetaData metadata;        //!< metadata object
    /*! Constructor for user labelled type parser */
    Parser(const std::string _label, std::function<T(State<X>&)> _f, std::vector<ParserMetaData const*> _children = {}) : f(_f), metadata(ParserMetaData(_label, _children)) {}
    /*! Construtor for library standard type parser */
    Parser(const parser_type _type, std::function<T(State<X>&)> _f, std::vector<ParserMetaData const*> _children = {}): f(_f), metadata(ParserMetaData(_type, _children)) {}

    /*! Perform the parse given a state */
    T parse(State<X> &s) {
      State<X> _s = s;
      try {
        return f(s);
      } catch (std::vector<State<X>> &e) {
        _s.fail(metadata.uuid);
        e.push_back(_s);
        throw e;
      }
    }
    
    /*! Method chain to generate sequential parser from current parser */
    template <typename U, typename V>
    std::shared_ptr<Parser<V,X>> seq(const std::shared_ptr<Parser<U,X>> second, const std::function<V(T,U)> g) {
      return std::make_shared<Parser<V,X>>(SEQ, [this,second,g](State<X> &s) -> V {
        return g(this->parse(s), second->parse(s));
      }, std::vector<ParserMetaData const*>{ &this->metadata, &second->metadata });
    }
  };
}
//TODO metadata is graph via children as adjlist
//TODO to print metadata stack, will need to make mapping from uuid to metadata
  // each State in stack trace will lookup the mapping