#include <bits/stdc++.h>

#define NULL_ERROR cout<<"Null pointer exception\n";
#define NOT_TOKEN ((1<<29) + 1)
#define OR_TOKEN  ((1<<29) + 2)
#define AND_TOKEN ((1<<29) + 3)
#define VAR_TOKEN ((1<<29) + 4)
using namespace std;
ifstream in;
ofstream out;
struct node{
	int sym;	// and, or, not, variable.
	int id;		// if it's a variable, it'll have a variable id.
	node *l;	// left child
	node *r;   	// right child
	node *p;	// parent node
	bool is_left;	// true if it's the left node
    bool visited;
};


// TODO: don't forget to initialize these vars
int VAR_ID;
map<string,int> name_to_id; 			//variable name to variable id mapping
map<int,string> id_to_name;   			//variable id to variable name mapping

void print_node(node *r){
    if(  r == NULL ){
        cout<<"NULL\n";
    }
    if(r->sym == OR_TOKEN){
        cout<<"OR\n";
    }
    else if(r->sym == NOT_TOKEN){
        cout<<"NOT\n";
    }
    else if(r->sym == AND_TOKEN){
        cout<<"AND\n";
    }
    else if(r->sym == VAR_TOKEN){
        if(id_to_name.find(r->id) == id_to_name.end())
            cout<<"random token\nERROR\n";
        cout<<(id_to_name[r->id])<<endl;
    }
    else{
        cout<<"error\n";
    }
 }


node *find_top_node(node *cur){
    if(cur->p != NULL)
        return find_top_node(cur->p);
    else return cur;
}

string tree_to_string(node *root){
    if(root->sym == OR_TOKEN){
        string l = tree_to_string(root->l);
        string r = tree_to_string(root->r);
        return "( "  + l + " OR " + r + " )";
    }
    else if(root->sym == NOT_TOKEN){
        string r = tree_to_string(root->r);
        return "( NOT " + r + " )";
    }
    else if(root->sym == AND_TOKEN){
        string l = tree_to_string(root->l);
        string r = tree_to_string(root->r);
        return "( "  + l + " AND " + r + " )";
    }
    else if(root->sym == VAR_TOKEN){
        return "( " + id_to_name[root->id] + " )";
    }
}

node *change_to_formula_tree(string input){
	if(input.length() == 0)
		return NULL;
	else{
		int i = 0;
		while(input[i] == ' '){
			++i;
			if(i == input.length()){
				cout<<"SOME PROBLEM\n";
				break;
			}
		}
		if(input[i] == 'N'){
			node *cur = new node;
			cur->sym = NOT_TOKEN;
			while(input[i] != ' '){
				i++;
				if( i == input.length()){
					cout<<"Some problem\n";
					break;
				}
			}
			string temp = "";
			for(int j = i; j < input.length(); ++j ) {
				temp += input[j];
			}
			cur->r = change_to_formula_tree(temp);
			cur->l = NULL;
			if(cur->r != NULL)
                cur->r->p = cur;
            else{
                cout<<"something went wrong\n";
                return NULL;
            }
            cur->p = NULL;
			return cur;
		}
		else if(input[i] == 'x'){
			string var_name = "";
			while(input[i] != ' '){
				var_name += input[i];
                i++;
				if( i == input.length()){
					//cout<<"problem occurred\n";
					break;
				}
			}
			node *cur = new node;
			if(name_to_id.find(var_name) == name_to_id.end()){
				name_to_id[var_name] = VAR_ID;
				id_to_name[VAR_ID] = var_name;
				cur->id = VAR_ID++;
			}
			else{
				cur->id = name_to_id[var_name];
			}
			cur->sym = VAR_TOKEN;
			cur->l = NULL;
			cur->r = NULL;
			cur->p = NULL;
			return cur;
		}
		else if(input[i] == '('){
			stack<char> s;
			s.push('(');
			int start = i+1;
			while(!s.empty()){
				i++;
				if( i == input.length() ){
					cout<<"THERE'S SOME PROBLEM IN INPUT\n";
					return NULL;
				}
				if(input[i] == ')'){
					s.pop();
				}
				else if(input[i] == '('){
					s.push('(');
				}
			}
			string f1 = "";
			for(int j = start; j < i; ++j){
				f1 += input[j];
			}
			i++;
			if( i == input.length() )
				return change_to_formula_tree(f1);
			while(input[i] == ' '){
				i++;
				if( i == input.length() ){
					return change_to_formula_tree(f1);
				}
			}
			string token = "";
			while(input[i] != ' '){
				token += input[i];
				i++;
			}

			string f2 = "";
			for(int j = i; j < input.length(); ++j){
				f2 += input[j];
			}


			node *cur = new node;
			if( token == "AND"){
				cur->sym = AND_TOKEN;
			}
			else if( token == "OR"){
				cur->sym = OR_TOKEN;
			}

			node *l_child = change_to_formula_tree(f1);
			l_child->p = cur;
			l_child->is_left = true;
			cur->l = l_child;

			node *r_child = change_to_formula_tree(f2);
			r_child->p = cur;
			r_child->is_left = false;
			cur->r = r_child;

			cur->p = NULL;
			return cur;
		}
		else{
			cout<<"THERE IS SOME PROBLEM WITH THE INPUT FORMULA\n";
			return NULL;
		}
	}
}


// TODO: these 3 functions haven't been tested yet!
void left_rotate(node *root){
	if( root == NULL ){
		NULL_ERROR;
		return;
	}
	if(root->l == NULL){
		NULL_ERROR;
		return;
	}
	else if(root->r == NULL){
		NULL_ERROR;
		return;
	}
	else{
		if( root->is_left && root->p != NULL ){
			root->p->l = root->r;
			root->r->is_left = true;
		}
		else if( root->p != NULL ){
			root->p->r = root->r;
			root->r->is_left = false;
		}
		node *right_child = root->r;
		root->r = root->r->l;
		root->r->p = root;
		root->r->is_left = false;
		right_child->p = root->p;
		root->p = right_child;
		right_child->l = root;
		root->is_left = true;
	}
}

void order_tree(node *root,stack<string> &rules);
void right_rotate(node *root,stack<string> &rules){
	if( root == NULL ){
		NULL_ERROR;
		return;
	}
	if(root->l == NULL){
		NULL_ERROR;
		return;
	}
	else if(root->r == NULL){
		NULL_ERROR;
		return;
	}
	else{
        //print_node(root);
        order_tree(find_top_node(root),rules);
		if( root->is_left && (root->p != NULL )){
			root->p->l = root->l;
			//print_node(root);
			root->l->is_left = true;
		}
		else if( root->p != NULL ){
			root->p->r = root->l;
			root->l->is_left = false;
		}

		node *left_child = root->l;
		left_child->p = root->p;

		root->l = left_child->r;
		root->l->p = root;
		root->l->is_left = true;
		left_child->r = root;
		root->p = left_child;
		root->is_left = false;
	}
}

void swap_children(node *root){
	if( root == NULL ){
		NULL_ERROR;
		return;
	}
	if(root->l == NULL){
		NULL_ERROR;
		return;
	}
	else if(root->r == NULL){
		NULL_ERROR;
		return;
	}
	node *temp = root->l;
	root->l = root->r;
	root->r = temp;
	root->l->is_left = true;
	root->r->is_left = false;
}

void set_visited_false(node *root){
    if(root == NULL)
        return;
    root->visited = false;
    set_visited_false(root->l);
    set_visited_false(root->r);
}


//assuming each of the formula starts with a '('.
string reverse_imply(string &s){
	string temp = "";
	int i = 0;
	string rule_name = "";
	while( s[i] != '('){
		rule_name += s[i++];
		if( i == s.length()){
			cout<<"there's some problem printing the formula\n";
		}
	}
	while( i < s.length() ){
		if(s[i] == '='){
			break;
		}
		else{
			temp += s[i++];
		}
	}
	i += 2;
	string temp2 = "";
	while(i < s.length())
		temp2 += s[i++];
	string ans = "";

	for( i = 0; i < rule_name.length(); ++i)
		ans += rule_name[i];
	i = 0;
	while(temp2[i] == ' ')++i;
	for( ; i < temp2.length(); ++i){
		if(temp2[i] == '\n')
			break;
		ans += temp2[i];
	}
	ans += " =";
	ans += "> ";
	for( i = 0; i < temp.length(); ++i)
		ans += temp[i];
	ans[ans.length()-1] = '\n';
	return ans;
}

void print_rules(stack<string> &s, bool reverse ){
    if(s.empty())
        return;
    if(reverse){
        string current = s.top();
        s.pop();
        print_rules(s,reverse);
        out<<current<<endl;
    }
    else{
        string current = s.top();
        s.pop();
        current = reverse_imply(current);
        out<<current<<endl;
        print_rules(s,reverse);
    }
}

// TODO: this is wrong implementation
void order_tree(node *root,stack<string> &rules){
	if(root == NULL)
		return;
	if(root->sym == AND_TOKEN || root->sym == OR_TOKEN){
		if(root->l->sym == VAR_TOKEN && root->r->sym == VAR_TOKEN ){
			string name1;
			string name2;
			name1 = id_to_name[root->l->id];
			name2 = id_to_name[root->r->id];
			if(name1.length() == 0 || name2.length() == 0 ){
				cout<<" problem in order tree\n";
			}
			else{
				if(name1 > name2){
					//note down the rule;
					string rule = "Using commutative law:\n";
					string formula1 =  tree_to_string(find_top_node(root));
					swap_children(root);
					string formula2 =  tree_to_string(find_top_node(root));
					for(int i = 0; i < formula1.length(); ++i ){
						rule += formula1[i];
					}
					rule += " => ";
					for(int i = 0; i < formula2.length(); ++i ){
						rule += formula2[i];
					}
					rule += "\n";
					rules.push(rule);
				}
			}
		}
		//here might be a null pointer exception (analyze if condition)
		else if(root->l->sym == VAR_TOKEN && root->r->sym == NOT_TOKEN && root->r->r->sym == VAR_TOKEN){
			string name1;
			string name2;
			name1 = id_to_name[root->l->id];
			name2 = id_to_name[root->r->r->id];
			if(name1.length() == 0 || name2.length() == 0 ){
				cout<<" problem in order tree\n";
			}
			else{
				if(name1 > name2){
					//note down the rule;
					string rule = "Using commutative law:\n";
					string formula1 =  tree_to_string(find_top_node(root));
					swap_children(root);
					string formula2 =  tree_to_string(find_top_node(root));
					for(int i = 0; i < formula1.length(); ++i ){
						rule += formula1[i];
					}
					rule += " => ";
					for(int i = 0; i < formula2.length(); ++i ){
						rule += formula2[i];
					}	
					rule += "\n";	
					rules.push(rule);
				}
			}
		}
		else if(root->l->sym == NOT_TOKEN && root->r->sym == VAR_TOKEN && root->l->r->sym == VAR_TOKEN){
			string name1;
			string name2;
			name1 = id_to_name[root->l->r->id];
			name2 = id_to_name[root->r->id];
			if(name1.length() == 0 || name2.length() == 0 ){
				cout<<" problem in order tree\n";
			}
			else{
				if(name1 > name2){
					//note down the rule;
					string rule = "Using commutative law:\n";
					string formula1 =  tree_to_string(find_top_node(root));
					swap_children(root);
					string formula2 =  tree_to_string(find_top_node(root));
					for(int i = 0; i < formula1.length(); ++i ){
						rule += formula1[i];
					}
					rule += " => ";
					for(int i = 0; i < formula2.length(); ++i ){
						rule += formula2[i];
					}		
					rule += "\n";
					rules.push(rule);
				}
			}
		}

		else if(root->l->sym == NOT_TOKEN && root->r->sym == NOT_TOKEN && root->l->r->sym == VAR_TOKEN && root->r->r->sym == VAR_TOKEN){
			string name1;
			string name2;
			name1 = id_to_name[root->l->r->id];
			name2 = id_to_name[root->r->r->id];
			if(name1.length() == 0 || name2.length() == 0 ){
				cout<<" problem in order tree\n";
			}
			else{
				if(name1 > name2){
					//note down the rule;
					string rule = "Using commutative law:\n";
					string formula1 =  tree_to_string(find_top_node(root));
					swap_children(root);
					string formula2 =  tree_to_string(find_top_node(root));
					for(int i = 0; i < formula1.length(); ++i ){
						rule += formula1[i];
					}
					rule += " => ";
					for(int i = 0; i < formula2.length(); ++i ){
						rule += formula2[i];
					}	
					rule += "\n";	
					rules.push(rule);
				}
			}
		}

	}
	order_tree(root->l,rules);
	order_tree(root->r,rules);
}

// TODO: handle the case for not here!
void swap_leaves(node *a, node *b){
	if(a->l != NULL || a->r != NULL || b->l != NULL || b->r != NULL){
		cout<<"Not a leaf error\n";
		return;
	}

	if(a->sym != VAR_TOKEN || b->sym != VAR_TOKEN){
		cout<<"Not a variable. Can't be swapped\n";
		return;
	}
	
	if(a->p->sym == NOT_TOKEN){
		a = a->p;
	}
	if(b->p->sym == NOT_TOKEN){
		b = b->p;
	}
	node *pa = a->p;
	node *pb = b->p;
	bool a_left = a->is_left;
	bool b_left = b->is_left;
	b->p = pa;
	a->p = pb;
	if(a_left){
		pa->l = b;
		b->is_left = true;
	}
	else{
		pa->r = b;
		b->is_left = false;
	}
	if(b_left){
		pb->l = a;
		a->is_left = true;
	}
	else{
		pb->r = a;
		a->is_left = false;
	}
}

void get_leaves(node *r, vector<node *> &v){
	if(r == NULL ){
		cout<<"there's some problem in formula tree\n";
		return;
	}
	if(r->l == NULL && r->r == NULL){
		v.push_back(r);
		return;
	}
	else{
		if(r->l != NULL){
			get_leaves(r->l,v);
		}
		get_leaves(r->r,v);
	}
}

void sort_nodes(vector<node *> a){
	int n = a.size();
	for(int i = 0; i < n; ++i){
		for(int j = i - 1; j >= 0; --j){
			if(id_to_name[a[j]->id] > id_to_name[a[j+1]->id]){
				swap_leaves(a[j],a[j+1]);
				node *temp = a[j];
				a[j] = a[j+1];
				a[j+1] = temp;
			}
		}
	}
}

void sort_tree(node *root){
	root = find_top_node(root);
	vector<node *> arr;
	get_leaves(root,arr);
	sort_nodes(arr);
}


bool compare_tree(node *r1, node *r2){
	if(r1 == NULL && r2 == NULL)
		return true;
	else if(r1 == NULL || r2 == NULL)
		return false;
	else{
		if(r1->sym == r2->sym){
			if(r1->sym == VAR_TOKEN){
				if(r1->id == r2->id){
					return compare_tree(r1->l,r2->l) && compare_tree(r1->r,r2->r);
				}
				else return false;
			}
			else return compare_tree(r1->l,r2->l) && compare_tree(r1->r,r2->r);
		}
		else return false;
	}
}

void convert_to_good_tree(node *root, stack<string> &s){
	if(root == NULL)
		return;
    if(root->visited)
        return;
    root->visited = true;
	convert_to_good_tree(root->r,s);
	if(root->sym == OR_TOKEN || root->sym == AND_TOKEN){
		if(root->l->sym == root->sym){
            string op = "Using associative law:\n";
            op += tree_to_string(find_top_node(root));
			right_rotate(root,s);
            op += " => ";
            string temp = tree_to_string(find_top_node(root));
            for(int i=0;i<temp.length(); ++i){
                op += temp[i];
            }
            op += "\n";
            s.push(op);
            convert_to_good_tree(root->p,s);
            return;
		}
	}
	convert_to_good_tree(root->l, s);
}

int main(){
	in.open("assoc_test.txt");
	out.open("output.txt");
	string formula_string1, formula_string2;
	getline(in,formula_string1);
	getline(in,formula_string2);
	node *f1 = change_to_formula_tree(formula_string1);
    node *f2 = change_to_formula_tree(formula_string2);
    string converted1 = tree_to_string(f1);
    string converted2 = tree_to_string(f2);
    //cout<<"Formula1:"<<converted1<<endl;
    //cout<<"Formula2:"<<converted2<<endl;
    stack<string> rules1;
    stack<string> rules2;
    order_tree(f1,rules1);
    order_tree(f2,rules2);
    set_visited_false(f1);
    set_visited_false(f2);
    f1 = find_top_node(f1);
    f2 = find_top_node(f2);
    convert_to_good_tree(f1,rules1);
    convert_to_good_tree(f2,rules2);
    sort_tree(f1);
   	sort_tree(f2);
   	f1 = find_top_node(f1);
   	f2 = find_top_node(f2);
    string convert_one = tree_to_string(f1);
    string convert_two = tree_to_string(f2);
    cout<<convert_one<<endl;
    cout<<convert_two<<endl;
    bool is_equal = compare_tree(f1,f2);
    if(!is_equal){
    	cout<<"Formulae don't match!\n";
    }
    else{
    	print_rules(rules1,true);
    	print_rules(rules2,false);
	}
    return 0;
}
