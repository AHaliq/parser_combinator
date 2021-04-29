#pragma once
#include <string>
#include <istream>

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

// parser data structures
///////////////////////////////////////////////////////////////////////////////

namespace parser::state {
  static const std::string STATE_NOT_FAILED_LABEL = "<not failed>"; //!< label when state not in failure state
  
  /*! State object virtual class */
  class State {
  protected:
    int i;                      //!< index to currently to be consumed character
  private:
    bool failed;                //!< flag to determine failure
    std::string failure_label;  //!< label for parser failures
  public:
    /*!
     * Construct state with pointer to source string
     */
    State() : i(0), failed(false) {}
    /*!
     * Advance the currently consumed character (must be implemented by inheriting class)
     * @return consumed character
     */
    virtual const char adv() { return 0; }
    /*!
     * Set state to failure with accompanying label
     * @param label failure label
     */
    void fail(const std::string &&label) {
      failed = true;
      failure_label = label;
    }
    /*!
     * Get failure flag
     * @return flag
     */
    const bool has_failed() { return failed; }
    /*!
     * Get failure label
     * @return failure label
     */
    const std::string& get_fail() {
      return failed ? failure_label : STATE_NOT_FAILED_LABEL;
    }
  };

  /*! State using std::string as source */
  class StateString : State {
  private:
    std::string *src;           //!< pointer to source string to be parsed
  public:
    StateString(std::string *_src) : State(), src(_src) {}
    const char adv() override {
      if (i >= src->size()) throw std::vector<State>();
      return src->at(i++);
    }
  };

  /*! State using std::istream as source */
  class StateIStream : State {
  private:
    std::istream *src;          //!< pointer to input stream used to parse
  public:
    StateIStream(std::istream *_src) : State(), src(_src) {}
    const char adv() override {
      char c;
      try {
        if (i != src->tellg()) src->seekg(i);
        *src >> c;
        if (c == 0) throw std::vector<State>();
        i++;
        return c;
      } catch(std::exception e) {
        throw std::vector<State>();
      }
    }
  };
}

