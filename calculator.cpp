#include <iostream>
#include <iomanip>
#include <math.h>
#include <vector>

using namespace std;
// TODO: add history functionality
// TODO: clean up commented out code,
// TODO: set up / allow cmdline args
// TODO: finish setting up verbosity
class Calculator {

public:
  Calculator() : m_start(0), m_stop(0), m_verbose(0), m_eqn{} {
    cout << "Welcome to Calculator.cpp!\nEnter an expression or enter 'quit' to exit.\n";
  }

  void run() {
    string input("");
    bool proceed(true);
    while (proceed) {
      cout << "\nEnter here: ";
      getline(cin, input);
      if (input == "quit")
        proceed = false;
      else {
        double answer = solve(input);
        cout << "Answer: " << answer << '\n';
      }
    }
    cout << "\nThank you for using Calculator.cpp! :)\n";
  }

  void show_steps(int verbose=-1) {
    if (verbose == -1)
      verbose = m_verbose;
    if (verbose)
      for (string s : m_eqn) 
        cout << "["+s+"]";
    cout << '\n';
  }

  double solve(string input, bool verbose=false) {
    input = remove_spaces(input);
    m_eqn = parse(input);
    show_steps(verbose);
    while (m_eqn.size() > 1) {
      simplify();
      show_steps(verbose);
    }
    return stod(m_eqn.at(0));
  }

private:
  string remove_spaces(string str){
    int len = str.length();
    string clean;
    for (int i(0); i < len; ++i){
      if (str[i] != ' ')
        clean += str[i];
    }return clean;
  }

  vector<string> parse(string input) {
    //cout << "parsing...\n";
    int len = input.length();
    string temp = "";
    vector <string> parsed;
    for (int i = 0; i < len; ++i) {
      if (isdigit(input[i]) || input[i] == '.')
        temp += input[i];
      else {
        if (temp.length())
          parsed.push_back(temp);
        temp = input[i];
        parsed.push_back(temp);
        temp = "";
      }
    }
    if (temp.length() > 0)
      parsed.push_back(temp);
    return eval_bin_minus(parsed);
  }

  vector<string> eval_bin_minus(vector<string> parsed) {
    vector<string> fin = parsed;
    int j = 0;;
    for (int i = 0; i < parsed.size(); ++i, ++j) {
      if (parsed.at(i) == "-") {
        if (i == 0/* && parsed.at(i+1) == "("*/) {
          fin.at(0) = "_";
          /*fin.at(0) = "-1";
          fin.insert(fin.begin() + ++j, "*");*/
        }
        else if (!isdigit(parsed.at(i-1)[0]) && parsed.at(i-1) != ")") {
          fin.at(j) = "_";/*
          fin.erase(fin.begin()+j);
          fin.at(j) = "-" + parsed.at(++i);*/
        }
      }
    }
    return fin;
  }
  double eval_un_exp(char op, double side) {
    switch (op) {
    case '_':
      return -side;
    }return 0;

  }
  double eval_bin_exp(char op, double lhs, double rhs) {
    //cout << "evaluating..." + to_string(lhs) + op + to_string(rhs) + "\n";
    switch (op) {
    case '^':
      return pow(lhs, rhs);
    case '*':
      return lhs * rhs;
    case '/':
      return lhs / rhs;
    case '+':
      return lhs + rhs;
    case '-':
      return lhs - rhs;
    } return 0;
  }

  int get_op_idx(string op, int dir=1) {
    string op1, op2;
    op1 += op[0];
    op2 += op[1];
    //cout << "searching for " + op + "\n";
    int idx = -1;
    int start = m_start;
    int stop = m_stop;
    switch (dir) {
    case -1:
      swap(start, stop);
      for (int i = start; i >= stop; --i) {
        if (m_eqn.at(i) == op1 || m_eqn.at(i) == op2)
          idx = i;
      } break;
    case 1:
      for (int i = start; i <= stop; ++i) {
        if (m_eqn.at(i) == op1 || m_eqn.at(i) == op2)
          idx = i;
      } break;
    }
    //cout << "found  " << op << " at idx: " << idx << '\n';
    return idx;
  }

  int set_frame() {
    m_start = 0;
    m_stop = m_eqn.size() - 1;
    int opidx = get_op_idx("(");
    int cpidx = get_op_idx(")", -1);
    if (opidx != -1 && cpidx != -1) {
      m_start = opidx;
      m_stop = cpidx;
    }return m_start - m_stop;
  }

  tuple<double, int, int> pemdas() {
    tuple<double, int, int> ins = {};
    vector<pair<string, int> > ops{{"_^", 1}, {"*/",-1}, {"+-",-1}};
    if (abs(set_frame()) == 2) {
      if (isdigit(m_eqn.at(m_start+1)[0]) || m_eqn.at(m_start+1).length() > 1) {
        ins = {stod(m_eqn.at(m_start+1)), m_start, m_stop};
        return ins;
      }
    }
    //cout << "frame set\n";
    int opidx = -1;
    string op =  "";
    for (int i = 0; i < ops.size() && opidx < 0; ++i)
      opidx = get_op_idx(ops.at(i).first, ops.at(i).second);
    double lhs = 0;
    double rhs = stod(m_eqn.at(opidx+1));
    double sol = 0;
    if (m_eqn.at(opidx) == "_") {
      sol = eval_un_exp('_', rhs);
      ins = {sol, opidx, opidx+1};
    }
    else {
      lhs = stod(m_eqn.at(opidx-1));
      sol = eval_bin_exp(m_eqn.at(opidx)[0], lhs, rhs);
      ins = {sol, opidx-1, opidx+1};
    }
    return ins;
  }

  vector<string> splice(tuple<double, int, int> ins) {
    vector<string> new_eqn = {};
    for (int i = 0; i < get<1>(ins); ++i)
      new_eqn.push_back(m_eqn.at(i));
    new_eqn.push_back(to_string(get<0>(ins)));
    for (int i = get<2>(ins)+1; i < m_eqn.size(); ++i)
      new_eqn.push_back(m_eqn.at(i));
    return new_eqn;
  }

  void simplify() {
    m_eqn = splice(pemdas());/*
    for (string s : m_eqn) cout << "["+s+"]";
    cout << "\n";*/
  }

  int m_start;
  int m_stop;
  int m_verbose;
  vector<string> m_eqn;
};

int main() {
  Calculator c;
  c.run();
  /* example usages of Calculator.solve()
  c.solve("-(-60--2-2)");
  c.solve("2+2-(-2^-3)^-2+-200");
  */
  return 0;
}
