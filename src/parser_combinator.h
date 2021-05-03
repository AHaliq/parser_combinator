#pragma once
#include <string>
#include <istream>
#include <streambuf>
#include <ios>
#include <functional>
#include <memory>
#include <cctype>

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
    /*! default constructor used by EitherAll */
    Both() {}
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
    Either() {}
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

  /*! Either both or one of either type */
  template <typename T1, typename T2>
  using EitherAll = Either<Both<T1, T2>, Either<T1, T2>>;

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

// mapping util functions
///////////////////////////////////////////////////////////////////////////////

namespace parser::maps
{
  using namespace parser::alg;

  // both mappers -------------------------------------------------------------

  /*! Returns first element of a pair */
  template <typename T, typename U>
  static T fst(const Both<T, U> &b) { return b.lx; }
  /*! Returns second element of a pair */
  template <typename T, typename U>
  static U snd(const Both<T, U> &b) { return b.rx; }
  /*!
   * Given a three element Both, return its middle element
   * @param b three element
   * @return middle value
   */
  template <typename T, typename U, typename V>
  static U get_mid(const Both<Both<T, U>, V> &b)
  {
    return b.lx.rx;
  }
  template <typename T, typename U, typename V>
  static U get_mid(const Both<T, Both<U, V>> &b)
  {
    return b.rx.lx;
  }

  // either mappers -----------------------------------------------------------

  /*!
   * Given an either where both sides are the same type,
   * return either which value
   * @param e either object
   * @return either value
   */
  template <typename T>
  static T get_either(const Either<T, T> &e)
  {
    return e.left ? e.lx : e.rx;
  }

  // either all mappers -------------------------------------------------------

  /*! Check if EitherAll has first value */
  template <typename T1, typename T2>
  static bool first_exists(EitherAll<T1, T2> &e) { return e.left || e.rx.left; }
  /*! Check if EitherAll has second value */
  template <typename T1, typename T2>
  static bool second_exists(EitherAll<T1, T2> &e) { return e.left || !e.rx.left; }
  /*! Trys to return first value of EitherAll */
  template <typename T1, typename T2>
  static T1 get_first(EitherAll<T1, T2> &e)
  {
    if (e.left)
      return e.lx.lx;
    return e.rx.lx;
  }
  /*! Trys to return second value of EitherAll*/
  template <typename T1, typename T2>
  static T2 get_second(EitherAll<T1, T2> &e)
  {
    if (e.left)
      return e.lx.rx;
    return e.rx.rx;
  }

  // general ------------------------------------------------------------------

  /*! map character vector to string */
  static std::string str_of_charvec(std::vector<char> res) {
    return std::string(res.begin(), res.end());
  }

  /*! map character vector to int */
  static int int_of_charvec(std::vector<char> res) {
    return std::stoi(str_of_charvec(res));
  }

  /*! char predicate determine if is digit */
  static const bool digit_pred(char c) { return std::isdigit(c); }
  /*! char predicate determine if is lowercase char */
  static const bool lower_pred(char c) { return std::islower(c); }
  /*! char predicate determine if is uppercase char */
  static const bool upper_pred(char c) { return std::isupper(c); }
  /*! char predicate determine if is alphabet */
  static const bool letter_pred(char c) { return std::isalpha(c); }
  /*! char predicate determine if is alphanumeric char */
  static const bool alphanum_pred(char c) { return std::isalnum(c); }
  /*! char predicate determine if is whitespace */
  static const bool space_pred(char c) { return std::isspace(c); }

}


// combiner util functions
///////////////////////////////////////////////////////////////////////////////

namespace parser::combiners
{
  /*! combine two characters into a string */
  static std::string str_of_chars(char a, char b) {
    return std::string(1, a) + std::string(1, b);
  }
  /*! concat two strings */
  static std::string str_concat(std::string a, std::string b) {
    return a + b;
  }
  /* fmap a vector and an element of its type using a function g */
  template <typename T, typename U, typename V>
  static std::function<std::vector<V>(std::vector<T>, U)>
  fmap(std::function<V(T,U)> g) {
    return [g](std::vector<T> xs, U y) -> std::vector<V> {
      std::vector<V> res;
      for(T x : xs)
        res.push_back(g(x, y));
      return res;
    };
  }
  /* cross product two vectors of the same type with a function g */
  template <typename T, typename U, typename V>
  static std::function<std::vector<V>(std::vector<T>, std::vector<U>)>
  product(std::function<V(T,U)> g) {
    return [g](std::vector<T> xs, std::vector<U> ys) -> std::vector<V> {
      std::vector<V> res;
      for (T x : xs)
        for (U y :ys)
          res.push_back(g(x,y));
      return res;
    };
  }
}

// parser state classes
///////////////////////////////////////////////////////////////////////////////

namespace parser::state
{
  /*! label when state not in failure state */
  const std::string STATE_NOT_FAILED_LABEL = "<not failed>";

  /*! Empty struct for default user data in state */
  struct empty
  {
  };

  /*! State object virtual class */
  template <typename X = empty>
  class State
  {
  protected:
    int i; //!< index to currently to be consumed character
  private:
    bool failed;   //!< flag to determine failure
    int fail_uuid; //!< uuid of failing parser
  public:
    X data; //!< user data
    /*!
     * Construct state with pointer to source string
     */
    State() : i(0), failed(false) {}
    /*!
     * Advance the currently consumed character.
     * must be implemented by inheriting class
     * @return consumed character
     */
    virtual const char adv()
    {
      throw std::vector<State<X>>();
    }
    /*!
     * Set state to failure with parser uuid
     * @param id failure uuid
     */
    void fail(const int id)
    {
      if (!failed)
      {
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
  class StateString : public State<X>
  {
  private:
    std::string *src; //!< pointer to source string to be parsed
  public:
    StateString(std::string *_src) : State<X>(), src(_src) {}
    const char adv() override
    {
      if (this->i >= src->size())
        throw std::vector<State<X>>();
      return src->at(this->i++);
    }
  };

  /*! State using std::istream as source */
  template <typename X = empty>
  class StateIStream : public State<X>
  {
  private:
    std::istream *src; //!< pointer to input stream used to parse
  public:
    StateIStream(std::istream *_src) : State<X>(), src(_src) {}
    const char adv() override
    {
      char c;
      if (this->i != src->tellg())
        src->seekg(this->i);
      *src >> c;
      if (c == 0)
        throw std::vector<State<X>>();
      this->i++;
      return c;
    }
  };

  /*! State using std::streambuf as source */
  template <typename X = empty>
  class StateStreamBuf : public State<X>
  {
  private:
    std::streambuf *src; //!< pointer to streambuf used to parse
  public:
    StateStreamBuf(std::streambuf *_src) : State<X>(), src(_src) {}
    const char adv() override
    {
      char c;
      if (this->i != src->pubseekoff(0, std::ios_base::cur))
        src->pubseekpos(this->i);
      c = src->sbumpc();
      if (c == -1)
        throw std::vector<State<X>>();
      this->i++;
      return c;
    }
  };
}

// parser class
///////////////////////////////////////////////////////////////////////////////

namespace parser
{
  using namespace state;
  using namespace alg;
  using namespace maps;

  /*! parser type values */
  enum parser_type
  {
    USER,
    SEQ,
    ALT,
    MAP,
    MANY
  };
  /*! mapping parser type to string */
  const std::string label_str[] = {"seq", "alt", "map", "many"};

  /*! class for parser metadata, info for parse errors and parse graphs */
  class ParserMetaData
  {
    using metadata_list = std::vector<ParserMetaData const *>;

  public:
    const int uuid;               //!< unique identifier
    const std::string user_label; //!< parser user label
    const parser_type type;       //!< parser type enum value
    const metadata_list children; //!< pointer to children metadata
    /*!
     * Constructor with user label
     * @param _label    user label
     * @param _children vector of pointers to children metadata
     */
    ParserMetaData(
        std::string _label,
        metadata_list _children = {}) : uuid(__COUNTER__),
                                        user_label(_label),
                                        type(USER),
                                        children(_children)
    {
    }
    /*!
     * Constructor with library parser type
     * @param _type     parser type enum value
     * @param _children vector of pointers to children metadata
     */
    ParserMetaData(
        parser_type _type,
        metadata_list _children = {}) : uuid(__COUNTER__),
                                        type(_type),
                                        children(_children)
    {
    }
  };

  using metadata_list = std::vector<ParserMetaData const *>;

  /*! Parser base class */
  template <typename T, typename X = empty>
  class Parser
  {
  private:
    const std::function<T(State<X> &)> f; //!< core function
  public:
    const ParserMetaData metadata; //!< metadata object
    /*! Constructor for user labelled type parser */
    Parser(
        const std::string _label,
        std::function<T(State<X> &)> _f,
        metadata_list _children = {}) : f(_f), metadata(ParserMetaData(_label, _children))
    {
    }
    /*! Construtor for library standard type parser */
    Parser(
        const parser_type _type,
        std::function<T(State<X> &)> _f,
        metadata_list _children = {}) : f(_f), metadata(ParserMetaData(_type, _children))
    {
    }

    /*!
     * Perform the parse given a state
     * @param s state object keeping track of parse state
     * @return  object corresponding to the parse
     */
    T parse(State<X> &s)
    {
      State<X> _s = s;
      try
      {
        return f(s);
      }
      catch (std::vector<State<X>> &e)
      {
        _s.fail(metadata.uuid);
        e.push_back(_s);
        throw e;
      }
    }

    // seq method chains ------------------------------------------------------

    /*!
     * Seq using user function
     * @param second  next parser to sequence to
     * @param g       combiner function for first and second results
     * @return        pointer to seq parser
     */
    template <typename U, typename V>
    std::shared_ptr<Parser<V, X>> seq(
        const std::shared_ptr<Parser<U, X>> second,
        const std::function<V(T, U)> g)
    {
      return std::make_shared<Parser<V, X>>(
          SEQ,
          [this, second, g](State<X> &s) -> V {
            return g(this->parse(s), second->parse(s));
          },
          metadata_list{&this->metadata, &second->metadata});
    }

    /*!
     * Seq using Both
     * @param second  next parser to sequence
     * @return        pointer to seq parser
     */
    template <typename U>
    std::shared_ptr<Parser<Both<T, U>, X>> seq(
        const std::shared_ptr<Parser<U, X>> second)
    {
      return seq<char, Both<T, U>>(
          second, [](T x, U y) { return Both<T, U>(x, y); });
    }

    /*!
     * Seq returning only the second parser result
     * @param second  next parsre to sequence
     * @return        pointer to seq parser
     */
    template <typename U>
    std::shared_ptr<Parser<U, X>> seq_ignore_fst(
        const std::shared_ptr<Parser<U, X>> second)
    {
      return std::make_shared<Parser<U, X>>(
          SEQ,
          [this, second](State<X> &s) -> U {
            this->parse(s);
            return second->parse(s);
          },
          metadata_list{&this->metadata, &second->metadata});
    }

    /*!
     * Seq returning only the first parser result
     * @param second  next parsre to sequence
     * @return        pointer to seq parser
     */
    template <typename U>
    std::shared_ptr<Parser<T, X>> seq_ignore_snd(
        const std::shared_ptr<Parser<U, X>> second)
    {
      return std::make_shared<Parser<T, X>>(
          SEQ,
          [this, second](State<X> &s) -> T {
            T res = this->parse(s);
            second->parse(s);
            return res;
          },
          metadata_list{&this->metadata, &second->metadata});
    }

    /*!
     * Operator shorthand for seq using Both
     * @param second  next parser to sequence
     * @return        pointer to seq parser
     */
    template <typename U>
    std::shared_ptr<Parser<Both<T, U>, X>> operator&(
        const std::shared_ptr<Parser<U, X>> second)
    {
      return seq(second);
    }

    /*!
     * Operator shorthand for seq_ignore_fst
     * @param second  next parser to sequence
     * @return        pointer to seq parser
     */
    template <typename U>
    std::shared_ptr<Parser<U, X>> operator>(
        const std::shared_ptr<Parser<U, X>> second)
    {
      return seq_ignore_fst(second);
    }

    /*!
     * Operator shorthand for seq_ignore_snd
     * @param second  next parser to sequence
     * @return        pointer to seq parser
     */
    template <typename U>
    std::shared_ptr<Parser<T, X>> operator<(
        const std::shared_ptr<Parser<T, X>> second)
    {
      return seq_ignore_snd(second);
    }

    // alt method chains ------------------------------------------------------

    /*!
     * Alt using Either
     * @param second  next parser to use if first fails
     * @return        pointer to alt parser
     */
    template <typename U>
    std::shared_ptr<Parser<Either<T, U>, X>> alt(
        const std::shared_ptr<Parser<U, X>> second)
    {
      return std::make_shared<Parser<Either<T, U>, X>>(
          ALT,
          [this, second](State<X> &s) -> Either<T, U> {
            State<X> _s = s;
            try
            {
              return Left<T, U>(this->parse(s));
            }
            catch (std::vector<State<X>> &e)
            {
              return Right<T, U>(second->parse(_s));
            }
          },
          metadata_list{&this->metadata, &second->metadata});
    }

    /*!
     * Alt using Either both of the same return type
     * @param second  next parser to use if first fails
     * @return        pointer to alt parser
     */
    std::shared_ptr<Parser<T, X>> alt(
        const std::shared_ptr<Parser<T, X>> second)
    {
      return std::make_shared<Parser<T, X>>(
          ALT,
          [this, second](State<X> &s) -> T {
            State<X> _s = s;
            try
            {
              return this->parse(s);
            }
            catch (std::vector<State<X>> &e)
            {
              return second->parse(_s);
            }
          },
          metadata_list{&this->metadata, &second->metadata});
    }

    /*!
     * Alt using EitherAll taking both or either results
     * @param second  next parser to use
     * @return        pointer to alt parser
     */
    template <typename U>
    std::shared_ptr<Parser<EitherAll<T, U>, X>> alt_both(
        const std::shared_ptr<Parser<U, X>> second)
    {
      return std::make_shared<Parser<EitherAll<T, U>, X>>(
          ALT,
          [this, second](State<X> &s) -> EitherAll<T, U> {
            State<X> _s = s;
            T res1;
            U res2;
            bool has1 = false;
            try
            {
              res1 = this->parse(s);
              has1 = true;
            }
            catch (std::vector<State<X>> &e)
            {
            }
            try
            {
              res2 = second->parse(_s);
              if (!has1)
                return Right<Both<T, U>, Either<T, U>>(Right<T, U>(res2));
            }
            catch (std::vector<State<X>> &e)
            {
              if (has1)
              {
                return Right<Both<T, U>, Either<T, U>>(Left<T, U>(res1));
              }
              else
              {
                throw e;
              }
            }
            return Left<Both<T, U>, Either<T, U>>(Both<T, U>(res1, res2));
          },
          metadata_list{&this->metadata, &second->metadata});
    }

    /*!
     * Alt using a vector collecting all possible alternative parses
     * @param second  next parser
     * @return        pointer to alt parser
     */
    std::shared_ptr<Parser<std::vector<T>, X>> alt_both(
        const std::shared_ptr<Parser<T, X>> second)
    {
      return std::make_shared<Parser<std::vector<T>, X>>(
          ALT,
          [this, second](State<X> &s) -> std::vector<T> {
            State<X> _s = s;
            std::vector<T> res;
            try
            {
              res.push_back(this->parse(s));
            }
            catch (std::vector<State<X>> &e)
            {
            }
            try
            {
              res.push_back(second->parse(_s));
            }
            catch (std::vector<State<X>> &e)
            {
            }
            return res;
          },
          metadata_list{&this->metadata, &second->metadata});
    }

    /*!
     * Operator shorthand for alt for both of same return type
     * @param second  next parser to use if first fails
     * @return        pointer to alt parser
     */
    std::shared_ptr<Parser<T, X>> operator|(
        const std::shared_ptr<Parser<T, X>> second)
    {
      return alt(second);
    }

    /*!
     * Operator shorthand for alt both parser of same type collected in vector
     * @param second  next parser to use
     * @return        pointer to alt parser
     */
    std::shared_ptr<Parser<std::vector<T>, X>> operator||(
        const std::shared_ptr<Parser<T, X>> second)
    {
      return alt_both(second);
    }

    // repetition method chains -----------------------------------------------

    /*!
     * Perform parse multiple times
     * folding the returning results into a single value
     * @param fold  fold function taking in current result and accumulation val
     * @param start starting accumulation value
     * @param min   minimum number of results to consider successful parse
     * @return      pointer to many parser
     */
    template <typename U>
    std::shared_ptr<Parser<U, X>> many(
        std::function<U(T, U)> fold, U start, int min = 0)
    {
      return std::make_shared<Parser<U, X>>(
          MANY,
          [this, fold, start, min](State<X> &s) -> U {
            State<X> _s = s;
            U cur = start;
            int count = 0;
            try
            {
              while (true)
              {
                cur = fold(parse(s), cur);
                _s = s;
                count++;
              }
            }
            catch (std::vector<State<X>> &e)
            {
              if (count < min)
              {
                throw e;
              }
              else
              {
                s = _s;
                return cur;
              }
            }
          },
          metadata_list{&this->metadata});
    }

    /*!
     * Perform parse multiple times
     * accumulating values into a vector
     * @param min   minimum number of results to consider successful parse
     * @return      pointer to many parser
     */
    std::shared_ptr<Parser<std::vector<T>, X>> many(int min = 0)
    {
      return std::make_shared<Parser<std::vector<T>, X>>(
          MANY,
          [this, min](State<X> &s) -> std::vector<T> {
            State<X> _s = s;
            std::vector<T> res;
            try
            {
              while (true)
              {
                res.push_back(parse(s));
                _s = s;
              }
            }
            catch (std::vector<State<X>> &e)
            {
              if (res.size() < min)
              {
                throw e;
              }
              else
              {
                s = _s;
                return res;
              }
            }
          },
          metadata_list{&this->metadata});
    }

    /*!
     * Sugar for many with minimum 1 result
     * @return  pointer to many parser
     */
    std::shared_ptr<Parser<std::vector<T>, X>> some()
    {
      return many(1);
    }

    /*!
     * Operator shorthand for many vector accumulation
     * @param min   minimum number of results to consider successful parse
     * @return      pointer to many parser
     */
    std::shared_ptr<Parser<std::vector<T>, X>> operator+(int min)
    {
      return many(min);
    }

    /*!
     * Operator shorthand for many vector accumulation with minimum 1 result
     * @return    pointer to many parser
     */
    std::shared_ptr<Parser<std::vector<T>, X>> operator++(int _)
    {
      return some();
    }

    // map method chains ------------------------------------------------------

    template <typename U>
    std::shared_ptr<Parser<U, X>> map(std::function<U(T)> g)
    {
      return std::make_shared<Parser<U, X>>(
          MAP,
          [this, g](State<X> &s) -> U {
            return g(parse(s));
          },
          metadata_list{&this->metadata});
    }

    template <typename U>
    std::shared_ptr<Parser<U, X>> operator%(std::function<U(T)> g)
    {
      return map(g);
    }

    // capture method chain ---------------------------------------------------
  };

  // shared_ptr operators -----------------------------------------------------

  template <typename T, typename U, typename X>
  std::shared_ptr<Parser<Both<T, U>, X>> operator&(
      const std::shared_ptr<Parser<T, X>> first,
      const std::shared_ptr<Parser<U, X>> second)
  {
    return first->seq(second);
  }

  template <typename T, typename U, typename X>
  std::shared_ptr<Parser<U, X>> operator>(
      const std::shared_ptr<Parser<T, X>> first,
      const std::shared_ptr<Parser<U, X>> second)
  {
    return first->seq_ignore_fst(second);
  }

  template <typename T, typename U, typename X>
  std::shared_ptr<Parser<T, X>> operator<(
      const std::shared_ptr<Parser<T, X>> first,
      const std::shared_ptr<Parser<U, X>> second)
  {
    return first->seq_ignore_snd(second);
  }

  template <typename T, typename X>
  std::shared_ptr<Parser<T, X>> operator|(
      const std::shared_ptr<Parser<T, X>> first,
      const std::shared_ptr<Parser<T, X>> second)
  {
    return first->alt(second);
  }

  template <typename T, typename X>
  std::shared_ptr<Parser<std::vector<T>, X>> operator||(
      const std::shared_ptr<Parser<T, X>> first,
      const std::shared_ptr<Parser<T, X>> second)
  {
    return first->alt_both(second);
  }

  template <typename T, typename X>
  std::shared_ptr<Parser<std::vector<T>, X>> operator||(
      const std::shared_ptr<Parser<std::vector<T>, X>> first,
      const std::shared_ptr<Parser<T, X>> second)
  {
    return std::make_shared<Parser<std::vector<T>, X>>(
        ALT,
        [first, second](State<X> &s) -> std::vector<T> {
          State<X> _s = s;
          std::vector<T> res = first->parse(s);
          try
          {
            res.push_back(second->parse(_s));
          }
          catch (std::vector<State<X>> &e)
          {
          }
          return res;
        },
        metadata_list{&first->metadata, &second->metadata});
  }

  template <typename T, typename X>
  std::shared_ptr<Parser<std::vector<T>, X>> operator+(
      const std::shared_ptr<Parser<T, X>> first, int min)
  {
    return first->many(min);
  }

  template <typename T, typename X>
  std::shared_ptr<Parser<std::vector<T>, X>> operator++(
      const std::shared_ptr<Parser<T, X>> first, int _)
  {
    return first->some();
  }

  template <typename T, typename U, typename X>
  std::shared_ptr<Parser<U, X>> operator%(
      const std::shared_ptr<Parser<T, X>> first,
      std::function<U(T)> g)
  {
    return first->map(g);
  }
}

namespace parser::primitives {

}
//TODO metadata is graph via children as adjlist
//TODO to print metadata stack, will need to make mapping from uuid to metadata
// each State in stack trace will lookup the mapping
//TODO change parser::alg::util to parser::maps, and make parser::combiners