#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <cctype>

// GENERIC DATA STRUCTURES ====================================================
template <typename T, typename U>
class Both
{
public:
  T lx;
  U rx;
  Both(T x, U y)
  {
    lx = x;
    rx = y;
  }
};

template <typename T, typename U>
class Either
{
public:
  bool left;
  T lx;
  U rx;
  template <class T_, class U_>
  friend Either<T_, U_> Left(T_ x);
  template <class T_, class U_>
  friend Either<T_, U_> Right(U_ x);
};

template <class T1, class T2>
Either<T1, T2> Left(T1 x)
{
  Either<T1, T2> e;
  e.left = true;
  e.lx = x;
  return e;
}
template <class T1, class T2>
Either<T1, T2> Right(T2 x)
{
  Either<T1, T2> e;
  e.left = false;
  e.rx = x;
  return e;
}

// PARSER DATA STRUCTURES =====================================================

class State
{
public:
  int i;
  std::string *source;
  State(std::string *s)
  {
    i = 0;
    source = s;
  }
};

class RenderItem
{
public:
  virtual std::string to_sexp(int sep = 0); /*
  virtual std::string to_stub(int sep = 0);
  virtual std::string to_latex(int sep = 0);*/
};

class Node
{
public:
  RenderItem data;
  std::vector<Node> children;
  Node(RenderItem val) { data = val; }
  Node() {}

  Node &add_child(Node &c)
  {
    children.push_back(c);
    return *this;
  }

  std::string to_sexp(int sep = 0)
  {
    std::string str = "( ";
    std::string sepstr(sep + 2, ' ');
    str += data.to_sexp(sep + 2);
    for (Node &c : children)
    {
      str += "\n" + sepstr;
      str += c.to_sexp(sep + 2);
    }
    str += " )";
    return str;
  } /*
  std::string to_stub(int sep = 0) {

  }
  std::string to_latex(int sep = 0) {

  }*/
};

// PARSER CLASS ===============================================================

template <typename T>
class Parser
{
public:
  std::function<T(State &)> f;
  std::string label;
  Parser(std::string l, std::function<T(State &)> _f)
  {
    f = _f;
    label = l;
  }
  Parser(std::function<T(State &)> _f)
  {
    f = _f;
    label = "";
  }

  T parse(State &s) const
  {
    State _s = s;
    try
    {
      return f(s);
    }
    catch (std::vector<State> &e)
    {
      e.push_back(_s);
      throw e;
    }
  }

  T parse(std::string str) const
  {
    State s(&str);
    return parse(s);
  }

  template <typename V, typename U>
  Parser<V> seq(std::string l, Parser<U> b, std::function<V(T &, U &)> g) const
  {
    return Parser<V>(l, [&, g, b](State &s) {
      return g(parse(s), b.parse(s));
    });
  }

  template <typename V, typename U>
  Parser<V> seq(Parser<U> b, std::function<V(T &, U &)> g) const
  {
    return seq("", b, g);
  }

  template <typename U>
  Parser<Both<T, U> > seq(std::string l, Parser<U> b) const
  {
    return seq(l, b, [](T &x, U &y) {
      return Both<T, U>(x, y);
    });
  }

  template <typename U>
  Parser<Both<T, U> > seq(Parser<U> b) const
  {
    return seq("", b);
  }

  template <typename U>
  Parser<Either<T, U> > alt(std::string l, Parser<U> b) const
  {
    return Parser<Either<T, U> >(l, [&, b](State &s) mutable {
      State _s = s;
      try
      {
        return Left<T, U>(parse(s));
      }
      catch (std::vector<State> e)
      {
        return Right<T, U>(b.parse(_s));
      }
    });
  }

  template <typename U>
  Parser<Either<T, U> > alt(Parser<U> b) const
  {
    return alt("", b);
  }

  template <typename U>
  Parser<U> map(std::function<U(T)> g) const
  {
    return Parser<U>([&, g](State &s) mutable {
      return g(parse(s));
    });
  }

  Parser<std::vector<T> > many(bool at_least_one = false) const
  {
    return Parser<std::vector<T> >([&, at_least_one](State &s) -> std::vector<T> {
      bool once = false;
      State _s = s;
      std::vector<T> res;
      try
      {
        while (true)
        {
          res.push_back(parse(s));
          _s = s;
          once = true;
        }
      }
      catch (std::vector<State> e)
      {
        if (at_least_one && !once)
        {
          throw std::vector<State>();
        }
        s = _s;
      }
      return res;
    });
  }

  Parser<std::vector<T> > some() const
  {
    return many(true);
  }
};

// PRIMITIVE PARSER AUXILLIARY FUNCTIONS ======================================

static const bool digit_pred(char c) { return std::isdigit(c); }
static const bool lower_pred(char c) { return std::islower(c); }
static const bool upper_pred(char c) { return std::isupper(c); }
static const bool letter_pred(char c) { return std::isalpha(c); }
static const bool alphanum_pred(char c) { return std::isalnum(c); }
static const bool space_pred(char c) { return std::isspace(c); }

static const int int_of_charvec(std::vector<char> res) { return std::stoi(std::string(res.begin(), res.end())); }
static const int int_of_either_ints(Either<int, int> res) { return res.left ? res.lx : res.rx; }

// PRIMITIVE PARSERS ==========================================================

static const Parser<char> sat(std::function<bool(char)> pred, std::string label = "")
{
  return Parser<char>(label, [pred](State &s) -> char {
    if ((*s.source).size() == s.i)
      throw std::vector<State>();
    char c = (*s.source).at(s.i);
    if (pred(c))
      s.i++;
    else
      throw std::vector<State>();
    return c;
  });
}

static const Parser<char> digit = sat(digit_pred, "digit");
static const Parser<char> lower = sat(lower_pred, "lower");
static const Parser<char> upper = sat(upper_pred, "upper");
static const Parser<char> letter = sat(letter_pred, "letter");
static const Parser<char> alphanum = sat(alphanum_pred, "alphanum");
static const Parser<char> space = sat(space_pred, "space");

static Parser<char> char_match(char c)
{
  return sat([c](char _c) -> bool { return _c == c; }, "char_match('" + std::string(1, c) + "')");
}
static Parser<std::string> string_match(std::string str)
{
  return Parser<std::string>("string_match('" + str + "')", [str](State &s) -> std::string {
    for (char _c : str)
    {
      if ((*s.source).size() == s.i)
        throw std::vector<State>();
      char c = (*s.source).at(s.i);
      if (c == _c)
        s.i++;
      else
        throw std::vector<State>();
    }
    return str;
  });
}

static const Parser<std::string> ident = Parser<std::string>("ident", [](State &s) {
  char x = lower.parse(s);
  std::vector<char> xs = alphanum.many().parse(s);
  xs.insert(xs.begin(), x);
  return std::string(xs.begin(), xs.end());
});

static const Parser<std::vector<char>> digit_some = digit.some();
static const Parser<int> nat = digit_some.map<int>(int_of_charvec);

static const Parser<int> intg_neg =
    Parser<int>([](State &s) {
      char_match('-').parse(s);
      int res = nat.parse(s);
      return -res;
    });
static const Parser<Either<int,int>> intg_either = intg_neg.alt("intg", nat);
static const Parser<int> intg = intg_either.map<int>(int_of_either_ints);

static const Parser<std::vector<char> > spaces = space.many();

template <typename T>
static Parser<T> token(Parser<T> p, std::string label = "")
{
  return Parser<T>("token('" + label + "')", [p](State &s) {
    spaces.parse(s);
    T res = p.parse(s);
    spaces.parse(s);
    return res;
  });
}
static const Parser<std::string> identifier = token<std::string>(ident, "identifier");
static const Parser<int> natural = token<int>(nat, "natural");
static const Parser<int> integer = token<int>(intg, "integer");
static Parser<std::string> symbol(std::string str)
{
  return token<std::string>(string_match(str), "symbol");
}

//TODO example calculator parser
//TODO basic AST parser
//TODO implement to_stub
//TODO implement to_latex
//TODO consider generalized state, adv() and clone()
// concrete as string pointer state, string stream state