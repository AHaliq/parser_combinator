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
  T lx;
  U rx;
  bool left;
  Either(T x)
  {
    lx = x;
    left = true;
  }
  Either(U x)
  {
    rx = x;
    left = false;
  }
};

// PARSER DATA STRUCTURES =====================================================

template <typename X>
class State
{
public:
  X data;
  int i;
  std::string *source;
  State(std::string *s) { source = s; }
};

class RenderItem
{
public:
  virtual std::string to_sexp(int sep = 0);/*
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

  Node& add_child(Node &c) {
    children.push_back(c);
    return *this;
  }

  std::string to_sexp(int sep = 0) {
    std::string str = "( ";
    std::string sepstr(sep + 2, ' ');
    str += data.to_sexp(sep + 2);
    for(Node &c : children) {
      str += "\n" + sepstr;
      str += c.to_sexp(sep + 2);
    }
    str += " )";
    return str;
  }/*
  std::string to_stub(int sep = 0) {

  }
  std::string to_latex(int sep = 0) {

  }*/
};

// PARSER CLASS ===============================================================

template <typename T, typename X = void>
class Parser
{
public:
  std::function<T &(State<X> &)> f;
  std::string label;
  Parser(std::string l, std::function<T &(State<X> &)> _f)
  {
    f = _f;
    label = l;
  }
  Parser(std::function<T &(State<X> &)> _f)
  {
    f = _f;
    label = "";
  }

  T &parse(State<X> &s)
  {
    State<X> _s = s;
    try
    {
      return f(s);
    }
    catch (std::vector<State<X> > &e)
    {
      e.push_back(_s);
      throw e;
    }
  }

  template <typename V, typename U>
  Parser<V, X> seq(std::string l, Parser<U, X> b, std::function<V &(T &, U &)> g)
  {
    return Parser<V, X>(l, [g, b, this](State<X> &s) {
      return g(parse(s), b.parse(s));
    });
  }

  template <typename V, typename U>
  Parser<V, X> seq(Parser<U, X> b, std::function<V &(T &, U &)> g)
  {
    return seq("", b, g);
  }

  template <typename U>
  Parser<Both<T, U>, X> seq(std::string l, Parser<U, X> b)
  {
    return seq(l, b, [=](T &x, U &y) {
      return Both<T, U>(x, y);
    });
  }

  template <typename U>
  Parser<Both<T, U>, X> seq(Parser<U, X> b)
  {
    return seq("", b);
  }

  template <typename U>
  Parser<Either<T, U>, X> alt(std::string l, Parser<U, X> b)
  {
    return Parser<Either<T, U>, X>(l, [b, this](State<X> &s) {
      State<X> _s = s;
      try
      {
        return Either<T, U>(parse(s));
      }
      catch (std::vector<State<X> > e)
      {
        return Either<T, U>(b.parse(_s));
      }
    });
  }

  template <typename U>
  Parser<Either<T, U>, X> alt(Parser<U, X> b)
  {
    return alt("", b);
  }

  template <typename U>
  Parser<U, X> map(std::function<U &(T &)> g)
  {
    return Parser<U, X>([g, this](State<X> &s) {
      return g(parse(s));
    });
  }

  Parser<std::vector<T>, X> many(bool at_least_one = false)
  {
    return Parser<std::vector<T>, X>([this, at_least_one](State<X> &s) {
      bool once = false;
      State<X> _s = s;
      std::vector<T> res;
      try {
        while(true) {
          res.push_back(parse(s));
          _s = s;
          once = true;
        }
      } catch (std::vector<State<X> > e) {
        if(at_least_one && !once) {
          throw std::vector<State<X> >();
        }
        s = _s;
      }
    });
  }

  Parser<std::vector<T>, X> some() {
    return many(true);
  }
};

// PRIMITIVE PARSERS ==========================================================

template <typename X>
Parser<char, X> sat(std::function<bool (char)> pred, std::string label = "") {
  return Parser<char, X>(label, [=](State<X> &s) {
    char c = (*s.source).at(s.i);
    if (pred(c)) {
      s.i++;
    } else {
      throw std::vector<State<X> >();
    }
  });
}

template <typename X>
const Parser<char, X> digit = sat<X>(std::isdigit, "digit");

template <typename X>
const Parser<char, X> lower = sat<X>(std::islower, "lower");

template <typename X>
const Parser<char, X> upper = sat<X>(std::isupper, "upper");

template <typename X>
const Parser<char, X> letter = sat<X>(std::isalpha, "letter");

template <typename X>
const Parser<char, X> alphanum = sat<X>(std::isalnum, "alphanum");

template <typename X>
const Parser<char, X> space = sat<X>(std::isspace, "space");

template <typename X>
Parser<char, X> char_match(char c) {
  return sat<X>([c](char _c) { return _c == c; }, "char_match('" + std::string(1,c) + "')");
}

template <typename X>
Parser<char, X> string_match(std::string str) {
  return Parser<char, X>("string_match('" + str + "')", [str](State<X> &s) {
    for(char _c : str) {
      char c = (*s.source).at(s.i);
      if (c == _c) {
        s.i++;
      } else {
        throw std::vector<State<X> >();
      }
    }
  });
}

template <typename X>
const Parser<std::string, X> ident = Parser<std::string, X>("ident", [](State<X> &s) {
  char x = lower<X>.parse(s);
  std::vector<char> xs = alphanum<X>.many().parse(s);
  xs.insert(xs.begin(), x);
  return std::string(xs.begin(), xs.end());
});

template <typename X>
const Parser<int, X> nat = digit<X>.some().map(std::stoi);

template <typename X>
const Parser<int, X> intg = char_match<X>('-').seq("intg", nat<X>).map([](int x) { return -x; }).alt(nat<X>);

template <typename X>
const Parser<std::vector<char>, X> spaces = space<X>.many();

template <typename X, typename T>
Parser<T, X> token(Parser<T, X> p, std::string label = "") {
  return Parser<T, X>("token('" + label + "')", [p](State<X> &s) {
    spaces<X>.parse(s);
    T res = p.parse(s);
    spaces<X>.parse(s);
    return res;
  });
}

template <typename X>
const Parser<std::string, X> identifier = token<std::string, X>(ident<X>, "identifier");

template <typename X>
const Parser<int, X> natural = token<int, X>(nat<X>, "natural");

template <typename X>
const Parser<int, X> integer = token<int, X>(intg<X>, "integer");

template <typename X>
Parser<std::string, X> symbol(std::string str) {
  return token<std::string, X>(string_match<X>(str), "symbol");
}

//TODO example calculator parser
//TODO basic AST parser
//TODO implement to_stub
//TODO implement to_latex
//TODO consider generalized state, adv() and clone()
  // concrete as string pointer state, string stream state