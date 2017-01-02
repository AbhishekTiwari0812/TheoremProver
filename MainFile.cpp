#include <set>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
using namespace std;

int SYM_TOKEN_START = 0xff;
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
int ID_DISTRIBUTER;
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
	
	token_to_sym[logical_and] = "\\&";
	sym_token["\\&"] = logical_and;


	unary_ops.insert(for_all);
	unary_ops.insert(exists);
	unary_ops.insert(logical_not);

	binary_ops.insert(logical_or);
	binary_ops.insert(logical_and);
}



enum NodeType{	VAR, SYM, PRED };

struct node{
	NodeType type;
	int val;		//token for latex symbol
	int id;
	node *left;		//null if unary operator
	node *right;
	node *parent;
	void in_order();
};

struct variable{
	int id;
	int value;
};

struct predicate{
	int id;
	int arity;
	vector<vector<int> > v;	//list of variable tuples for which the predicate is true
};

void node::in_order(){
	if(this->left){
		this->left->in_order();
	}
	//TODO: add working of current
	cout<<token_to_sym[this->val]<<" id: "<<this->id<<'\n';
	//cout<<token_to_sym[this->val]<<' ';
	if(this->right){
		this->right->in_order();
	}
	if(this->val==left_paren)
		cout<<" ) ";
}

//creates a tree representation of the given formula string
//returns the root of the tree
node *create_formula_tree(vector<int> &tokens,int start,int end){
	if(end == start){
		node *new_node = new node;
		new_node->val = tokens[start];
		new_node->left = NULL;
		new_node->id = ID_DISTRIBUTER++;
		new_node->right = NULL;
		new_node->type = VAR;
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
			node *new_node = new node;
			new_node->val = left_paren;
			new_node->type = SYM;
			new_node->id = ID_DISTRIBUTER++;
			new_node->right = create_formula_tree(tokens,start+1,end-1);
			new_node->right->parent = new_node;
			return new_node;
		}
		else{
			node *new_node = new node;
			new_node->type = SYM;
			new_node->id = ID_DISTRIBUTER++;
			new_node->val = tokens[right_matching_paren+1];
			new_node->left = create_formula_tree(tokens,start+1,right_matching_paren-1);
			new_node->right = create_formula_tree(tokens,right_matching_paren+1,end-1);
			new_node->right->parent = new_node;
			new_node->left->parent = new_node;
			return new_node;
		}

	}
	else if(unary_ops.find(tokens[start])!=unary_ops.end()){
		node *new_node =  new node;
		new_node->val = tokens[start];
		new_node->left = NULL;
		new_node->id = ID_DISTRIBUTER++;
		new_node->right = create_formula_tree(tokens,start+1,end);
		new_node->right->parent = new_node;
		new_node->type = SYM;
		return new_node;
	}
	/*
	else if(){
		//handle predicates
	}*/
	else if(token_to_sym.find(tokens[start]) != token_to_sym.end()){
		node *new_node =  new node;
		new_node->val = tokens[start];
		new_node->left = NULL;
		new_node->id = ID_DISTRIBUTER++;
		new_node->right = create_formula_tree(tokens,start+1,end);
		new_node->right->parent = new_node;
		new_node->type = VAR;
		return new_node;
	}
	else {
		cout<<"Error occured while parsing the tokens\n";
		return NULL;
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
			if(temp.length()==0){
				i++;
				continue;
			}
			/*if(predicate_token.find(temp)!=predicate_token.end()){
				ans.push_back(predicate_token[temp]);
				temp="";				
			}*/
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
	cout<<"Input string:\n";
	cout<<latex_formula<<endl;
	cout<<"Formula recognized by the program:\n";
	for(int i=0;i<tokens.size();++i){
		cout<<token_to_sym[tokens[i]]<<' ';
	}
	cout<<endl;
	return create_formula_tree(tokens,0,tokens.size()-1);
}


node *find_quantifier(node *root, int var_token){
	if(root == NULL){
		return NULL;
	}
	else if(root->val == for_all || root->val == exists){
		if(token_to_sym[root->right->val] == token_to_sym[var_token])
			return root;
		else return find_quantifier(root->parent,var_token);
	}
	else return find_quantifier(root->parent,var_token);
}

//finds the quantifiers to which the the current variable is bound
void find_binder(node *root){
	if(root == NULL)
		return;
	if(root->val >= SYM_TOKEN_START && root->val <= SYM_TOKEN){
		node *binder = find_quantifier(root,root->val);
		if(binder){
			cout <<  token_to_sym[root->val] <<" (id:"<<root->id<<") is bounded by:"; 
			cout << token_to_sym[binder->val] << "(id:" << binder->id<<")\n";
		}
		else{
			cout << token_to_sym[root->val] <<" (id:"<<root->id<<") is free variable\n"; 
 		}
	}
	find_binder(root->left);
	find_binder(root->right);
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
	cout<<"After parsing and reconstruction:\n";
	current_formula->head->in_order();
	cout<<'\n';
	return current_formula;
}


int main(){
	ID_DISTRIBUTER = 0;
    fill_symbols();
	formula *current_formula = read_formula();
	find_binder(current_formula->head);
	in.close();
	out.close();
	return 0;
}




