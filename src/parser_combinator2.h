#pragma once

// algebraic data structures
///////////////////////////////////////////////////////////////////////////////

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
private:
  bool left; //!< flag to indicate object contains left value
public:
  T1 *lx; //!< left value
  T2 *rx; //!< right value
  /*! destructor destroys both values */
  ~Either() {
    delete lx;
    delete rx;
  }
  /*!
   * Constructor for left value
   * @param x left value
   */
  template <typename T1_, typename T2_>
  friend Either<T1_, T2_>* Left(T1_ *x)
  {
    Either<T1, T2> *e = new Either<T1, T2>();
    e->left = true;
    e->lx = x;
    e->rx = nullptr;
    return e;
  }
  /*!
   * Constructor for right value
   * @param x right value
   */
  template <typename T1_, typename T2_>
  friend Either<T1_, T2_>* Right(T2_ *x)
  {
    Either<T1, T2> *e = new Either<T1, T2>();
    e->left = false;
    e->lx = nullptr;
    e->rx = x;
    return e;
  }
};