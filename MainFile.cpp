#include <iostream>
#include <fstream>
#include <map>
#include <vector>
using namespace std;

int SYM_TOKEN = 0xff;
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

map<int,string> token_to_sym;	//key => encoded symbol, value => latex format of formula
map<string,int> sym_token;	//key => latex format of formula, value => encoded symbol
set<int> unary_ops;
set<int> binary_ops;

//creates a map of latex symbols to int consts (to improve comparison and matching time)
void fill_symbols(){
	token_to_sym[for_all] = "\\forall";
	sym_token["\\forall"]=for_all;

	token_to_sym[exists] = "\\exists";
	sym_token["\\exists"] = exists;

	token_to_sym[belongs] = "\\in";
	sym_token["\\in"] = belongs;

	token_to_sym[such_that] = "\\ni";
	sym_token["\\ni"] = such_that;

	token_to_sym[prop_subset] = "\\subset";
	sym_token["\\subset"] = prop_subset;

	token_to_sym[subset] = "\\subseteq";
	sym_token["\\subseteq"] = subset;

	token_to_sym[implies] = "\\Rightarrow";
	sym_token["\\Rightarrow"] = implies;

	token_to_sym[both_implies] = "\\Leftrightarrow";
	sym_token["\\Leftrightarrow"] = both_implies;

	token_to_sym[logical_not] = "\\neg";
	sym_token["\\neg"] = logical_not;

	token_to_sym[empty_set] = "\\emptyset";
	sym_token["\\emptyset"] = empty_set;

	token_to_sym[equivalent] = "\\equiv";
	sym_token["\\equiv"] = equivalent;

	token_to_sym[equal] = "=";
	sym_token["="] = equal;

	token_to_sym[left_paren] = "(";
	sym_token["("] = left_paren;

	token_to_sym[right_paren] = ")";
	sym_token[")"] = right_paren;
	
	unary_ops.insert(for_all);
	unary_ops.insert(exists);
	unary_ops.insert(logical_not);

	binary_ops.insert(logical_or);
	binary_ops.insert(logical_and);
}



enum NodeType{	VAR, SYM };

struct node{
	NodeType type;
	int val;		//token for latex symbol
	node *left;		//null if unary operator
	node *right;	
};


node *get_node(vector<int> &tokens,int start,int end){
	if(end == start){
		node *new_node = new node;
		new_node->val = tokens[start];
		new_node->left = NULL;
		new_node->right = NULL;
		return new_node;
	}
	else if(tokens[start]==left_paren){
		int count_left = 1;
		int right_matching_paren;
		for(int i=start+1;i<=end;++i){
			if(tokens[i]==right_paren){
				count_left--;

			}
			if(count_left==0){
				right_matching_paren = i;
				break;
			}
		}
		if(right_matching_paren==end){
			return get_node(tokens,start+1,end-1);
		}
		else{
			node *new_node = new node;
			new_node->type = SYM;
			new_node->val = tokens[right_matching_paren+1];
			new_node->left = get_node(tokens,start+1,right_matching_paren-1);
			new_node->righ = get_node(tokens,right_matching_paren+1,end-1);
			return new_node;
		}

	}
	else if(unary_ops.find(tokens[start])!=unary_ops.end()){

	}
	else{
		cout<<"There's some problem with the program\n";
	}
}

class formula{
public:
	string input;
	node *head;
	node *get_head(){
		return head;
	}
};


vector<int> tokenizer(string s){ //currently this function handles formula only if each symbol is seperated by spaces
	string temp = "";
	vector<int> ans;
	int i = 0;
	while(i < s.length()){
		if(s[i] == ' '){
			if(sym_token.find(temp) == sym_token.end()){
				sym_token[temp] = SYM_TOKEN;
				token_to_sym[SYM_TOKEN] = temp;
                ans.push_back(SYM_TOKEN);
				SYM_TOKEN++;
				temp="";
            }
			else{
				ans.push_back(sym_token[temp]);
				temp="";
			}
		}
		else
			temp += s[i];
		i++;
	}
	if(sym_token.find(temp) == sym_token.end()){
		sym_token[temp] = SYM_TOKEN;
		token_to_sym[SYM_TOKEN] = temp;
		ans.push_back(SYM_TOKEN);
		SYM_TOKEN++;
    }
	else
		ans.push_back(sym_token[temp]);
	return ans;
}

ofstream out;	//output-file writer
ifstream in;	//input file reader
node *parser(string &latex_formula){
	vector<int> tokens = tokenizer(latex_formula);
	cout<<"Formula recognized by the program:\n";
	for(int i=0;i<tokens.size();++i){
		cout<<token_to_sym[tokens[i]]<<' ';
	}
	cout<<endl;
}


formula *read_formula(){
	in.open("input.txt");
	string latex_formula; //formula in the latex format
	//only handles one line formula
	formula *current_formula;
	while(getline(in,latex_formula)){
		node *root = parser(latex_formula);
		current_formula = new formula;
		current_formula->head = root;
	}
	return current_formula;
}


int main(){
    fill_symbols();
	formula *current_formula = read_formula();
	in.close();
	out.close();
	return 0;
}
