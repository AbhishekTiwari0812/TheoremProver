#include <iostream>
#include <fstream>
#include <map>
using namespace std;

#define for_all 1
#define exists 2
#define empty_set 3
#define implies 4
#define both_implies 5
#define left_paren 6
#define right_paren 7
#define logical_and 8
#define logical_or 9
#define logical_not 10
#define belongs 11
#define such_that 12
#define prop_subset 13 //proper subset
#define subset 14
#define equivalent 15
#define equal 16
map<int,string> form_sym_mapper;	//key => encoded symbol, value => latex format of formula
map<string,int> sym_form_mapper;	//key => latex format of formula, value => encoded symbol

//creates a map of latex symbols to int consts (to improve comparison and matching time)
void fill_symbols(){
	form_sym_mapper[for_all] = "\\forall";
	sym_form_mapper["\\forall"]=for_all;

	form_sym_mapper[exists] = "\\exists";
	sym_form_mapper["\\exists"] = exists;

	form_sym_mapper[belongs] = "\\in";
	sym_form_mapper["\\in"] = belongs;

	form_sym_mapper[such_that] = "\\ni";
	sym_form_mapper["\\ni"] = such_that;

	form_sym_mapper[prop_subset] = "\\subset";
	sym_form_mapper["\\subset"] = prop_subset;

	form_sym_mapper[subset] = "\\subseteq";
	sym_form_mapper["\\subseteq"] = subset;

	form_sym_mapper[implies] = "\\Rightarrow";
	sym_form_mapper["\\Rightarrow"] = implies;

	form_sym_mapper[both_implies] = "\\Leftrightarrow";
	sym_form_mapper["\\Leftrightarrow"] = both_implies;

	form_sym_mapper[logical_not] = "\\neg";
	sym_form_mapper["\\neg"] = logical_not;

	form_sym_mapper[empty_set] = "\\emptyset";
	sym_form_mapper["\\emptyset"] = empty_set;

	form_sym_mapper[equivalent] = "\\equiv";
	sym_form_mapper["\\equiv"] = equivalent;

	form_sym_mapper[equal] = "=";
	sym_form_mapper["="] = equal;

	form_sym_mapper[left_paren] = "(";
	sym_form_mapper["("] = left_paren;

	form_sym_mapper[right_paren] = ")";
	sym_form_mapper[")"] = right_paren;
}


enum NodeType{	VAR, SYM };

struct node{
	NodeType type;
	int val;		//latex symbol
};



class formula{
	string input;
	node *head;
public:
	node *get_head(){
		return head;
	}
};

//TODO: rename this method namespace
node *parser(string &latex_formula,int start_index,int end_index){

}

ifstream in;	//input file reader
ofstream out;	//output-file writer
formula *read_formula(){
	in.open("input.txt");
	string latex_formula; //formula in the latex format
	in>>latex_formula;
}

void fill_symbols();		//creates mapping between latex symbols logical_and
int main(){
	formula *current_formula = read_formula();



	in.close();
	out.close();
	return 0;
}
