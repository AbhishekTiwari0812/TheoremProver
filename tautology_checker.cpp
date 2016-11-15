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
    bool var_neg; 		// true if neg(x) is present instead of x.
    node(){
    	this->l = NULL;
    	this->r = NULL;
    	this->p = NULL;
    	this->is_left = false;
    	this->visited = false;
    	this->var_neg = false;
    }
};

node *copy_node_except_pointers(node *cur){
	node *new_node = new node;
	new_node->sym = cur->sym;
	new_node->id = cur->id;
	new_node->l = NULL;
	new_node->r = NULL;
	new_node->p = NULL;
	new_node->is_left = cur->is_left;
	new_node->visited = cur->visited;
	new_node->var_neg = cur->var_neg;
	return new_node;
}

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
    	if(root->var_neg){
    		return "( NOT ( " + id_to_name[root->id] + " ) )";
    	}
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
				if(r1->id == r2->id && r1->var_neg == r2->var_neg){
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

// flag = true if not expansion happening
// false if it has stopped
void delete_node(node *c){
	if(!(c->sym == NOT_TOKEN || c->sym == VAR_TOKEN)){
		cout<<"Can not delete node\n";
		cout<<"some error\n";
		return;
	}
	else{
		node *p = c->p;
		node *r = c->r;
		if( p != NULL && r != NULL){
			if(c->is_left){
				p->l = r;
				r->p = p;
				r->is_left = true;
			}
			else{
				p->r = r;
				r->p = p;
				r->is_left = false;
			}
		}
		else if( p == NULL && r != NULL){
			r->p = NULL;
		}
		else if( r == NULL && p != NULL){
			if(c->is_left)
				p->l = NULL;
			else p->r = NULL;
		}
		else{
			cout<<"there is some error\n";
		}
		delete c;
	}
}
bool handle_special_not_case(node *&c){
    if(c->sym == NOT_TOKEN){
        c = c->r;
        c->p = NULL;
        return true;
    }
    return false;
}

void not_expand(node *root,bool flag){
	if(root == NULL)
		return;

	if(flag){
		if(root->sym == NOT_TOKEN){
			//cout<<"current: not";
			node *next = root->r;
			//cout<<"removing not before:"<<id_to_name[next->id]<<endl;
			delete_node(root);
			not_expand(next,false);
		}
		else if(root->sym == AND_TOKEN){
			root->sym = OR_TOKEN;
			//cout<<"currently at and";
			not_expand(root->l,true);
			not_expand(root->r,true);
		}
		else if(root->sym == OR_TOKEN){
			root->sym = AND_TOKEN;
			//cout<<"currently at or\n";
			not_expand(root->l,true);
			not_expand(root->r,true);
		}
		else if(root->sym == VAR_TOKEN){
            //cout<<"at a variable\n";
            //cout<<"var name"<<id_to_name[root->id]<<endl;
			root->var_neg = !root->var_neg;
		}

	}
	else{
		if(root->sym == NOT_TOKEN){
			node *next = root->r;
			delete_node(root);
			return not_expand(next, true);
		}
		else if(root->sym == AND_TOKEN || root->sym == OR_TOKEN){
			not_expand(root->l,false);
			not_expand(root->r,false);
		}
		else if(root->sym == VAR_TOKEN){
			return;
		}
		else{
			cout<<"There is some error in not expand. Sym not recognized\n";
			return;
		}
	}
}

node *copy_tree(node *current){
	if(current == NULL){
		NULL_ERROR;
		return NULL;
	}
	node *root = copy_node_except_pointers(current);
	if(current->sym == AND_TOKEN || current->sym == OR_TOKEN){
		root->l = copy_tree(current->l);
		root->r = copy_tree(current->r);
		root->l->p = root;
		root->r->p = root;
	}
	else if(current->sym == NOT_TOKEN){
		root->r = copy_tree(current->r);
		root->r->p = root;
	}
	root->p = NULL;
	return root;
}

node *distribute(node *root,stack<string> &rules){
	if(root->sym == AND_TOKEN){
		if(root->l->sym == OR_TOKEN){
			node *c = copy_tree(root->r);
			node *a = root->l->l;
			node *b = root->l->r;
			node *p = root->p;
			cout<<"before distribute:\n";
			string s = tree_to_string(find_top_node(root));
			cout<<s<<endl;
			node *new_root = copy_node_except_pointers(root->l);
			if( p != NULL){
				if(root->is_left){
					new_root->is_left = true;
					new_root->p = p;
					p->l = new_root;
				}
				else{
					new_root->is_left = false;
					new_root->p = p;
					p->r = new_root;
				}
			}
			else{
				new_root->p = NULL;
				new_root->is_left = false;
			}
			new_root->r = root;
			root->p = new_root;
			root->is_left = false;

			root->l = b;
			b->is_left = true;
			b->p = root;

			node *left_and = copy_node_except_pointers(root);
			new_root->l = left_and;
			left_and->p = new_root;
			left_and->is_left = true;

			left_and->l = a;
			a->p = left_and;
			a->is_left = true;

			left_and->r = c;
			c->p = left_and;
			c->is_left = false;

			distribute(left_and, rules);
			distribute(root, rules);

			cout<<"After distribute:\n";
			s = tree_to_string(find_top_node(root));
			cout<<s<<endl;
			return new_root;
		}
		else if(root->r->sym == OR_TOKEN){
			// using commutative law
			cout<<"before distribute:\n";
			string s = tree_to_string(find_top_node(root));
			cout<<s<<endl;
			swap_children(root);
			distribute(root,rules);
			cout<<"After distribute:\n";
			s = tree_to_string(find_top_node(root));
			cout<<s<<endl;
		}
		else{

		}
	}
	else if(root->sym == OR_TOKEN){
		distribute(root->l, rules);
		distribute(root->r, rules);
	}
	else if(root->sym == VAR_TOKEN){
	}
	else{
		cout<<"there's some problem in distribute function\n";
	}
}

void print_clauses(vector<vector<string> > &v){
	for(int i = 0; i < v.size(); ++i ){
		for(int j = 0; j < v[i].size(); ++j ){
			cout<<v[i][j]<<' ';
		}
		cout<<endl;
	}
}

bool negation(string s1, string s2){
    cout<<"testing:"<<s1<<' '<<s2<<endl;
	int l1 = s1.length();
	int l2 = s2.length();
	if(l1 > l2){
		string temp = s1;
		s1 = s2;
		s2 = temp;
	}
	l1 = s1.length();
	l2 = s2.length();
	cout<<"l1 = "<<l1<<" and l2= "<<l2<<endl;
	if(l1 != l2-1)
		return false;
	else{
		for(int i = 0; i < l1; ++i ){
			if(s1[i] != s2[i]){
                cout<<"here\n";
				return false;
			}
		}
		if(s2[l1] == '~'){
            cout<<"here3"<<endl;
			return true;
		}
		else{
            cout<<"here2"<<endl;
            return false;
		}
	}
}

pair<int,int> find_conflict(vector<vector<string> >&cls,int x, int y){
	for(int i = 0; i < cls.size(); ++i){
		for(int j = 0; j < cls[i].size(); ++j ){
			if( i == x && j == y)
				continue;
			else{
				if(negation(cls[i][j],cls[x][y])){
                    cout<<"returning from find_conflict:"<<i<<' '<<j<<endl;
					return make_pair(i,j);
				}
			}
		}
	}
	return make_pair(-1,-1);
}

bool test_contradiction(vector<vector<string> >&cls, int x){
	if(cls[x].size() != 1)
		return false;
	for(int i = 0; i < cls.size(); ++i ){
		if(cls[i].size() == 1){
			if(negation(cls[i][0],cls[x][0]))
				return true;
		}
	}
	return false;
}

bool merge_vector(vector<vector<string> > &cls,int x1, int x2, int y1, int y2){
	if(x1 == -1 || x2 == -1)return false;
	vector<string> helper;
	for(int i = 0; i < cls[x1].size(); ++i){
		if( i != y1 )
			helper.push_back(cls[x1][i]);
	}
	for(int i = 0; i < cls[x2].size(); ++i){
		if( i != y2 )
			helper.push_back(cls[x2][i]);
	}
	if( x2 < x1 ){
		int temp = x2;
		x2 = x1;
		x1 = temp;
	}
	cls.erase(cls.begin() + x2);
	int l1 = cls[x1].size();
	for(int i = l1 - 1 ; i >= 0; --i ){
		cls[x1].erase(cls[x1].begin()+i);
	}
	for(int i = 0; i < helper.size(); ++i ){
		cls[x1].push_back(helper[i]);
	}
	if(cls[x1].size() == 0)
        return true;
	return test_contradiction(cls,x1);
}

class cmp{
public:
    bool operator()(const vector<string> &a,const vector<string> &b)const{
        if(a.size() != b.size())
            return a.size() < b.size();
        for(int i = 0; i < a.size(); i++){
            if(a[i] != b[i])
                return a[i] < b[i];
        }
        return a[0] < b[0];
    }
};

vector<vector<string> > clean_formula(vector<vector<string> > cls){
    set<vector<string>,cmp> set_clause;
    for(int i = 0; i < cls.size(); ++i){
        sort(cls[i].begin(), cls[i].end() );
        set<string> s;
        for(int j = 0; j < cls[i].size(); ++j){
            s.insert(cls[i][j]);
        }
        vector<string> v;
        for(set<string>::iterator it = s.begin(); it != s.end(); ++it ){
            v.push_back((*it));
        }
        set_clause.insert(v);
    }
    vector<vector<string> > ans;
    for(set<vector<string> >::iterator it = set_clause.begin(); it != set_clause.end(); ++it){
        ans.push_back((*it));
    }
    return ans;
}

void resolution(vector<vector<string> > &v){
	bool contradicted = false;
	cout<<"number of clauses:"<<v.size()<<endl;
	for(int i = 0; i < v.size(); ++i ){
        cout<<"current row:"<<v[i][0]<<endl;
        cout<<"number of literals:"<<v[i].size()<<endl;
		for(int j = 0; j < v[i].size(); ++j ){
			cout<<"Testing for:"<<v[i][j]<<endl;
			pair<int,int> conflicting_pair = find_conflict(v, i, j);
			contradicted = merge_vector(v,i,conflicting_pair.first, j, conflicting_pair.second);
			if(contradicted){
				break;
			}
			v = clean_formula(v);
		}
		if(contradicted){
			cout<<"Given formula is a tautology.\n";
			return;
		}
	}
	if(!contradicted){
        cout<<"Couldn't find contradiction for ~( P => Q )\n";
    	cout<<"Given formula is not a tautology.\n";
    }
	return;
}

node *and_or_tree_join(node *a, node *b, bool is_and){
	node *new_node = new node;
	if(is_and)
		new_node->sym = AND_TOKEN;
	else
		new_node->sym = OR_TOKEN;
	new_node->p = NULL;
	new_node->l = a;
	new_node->r = b;
	a->is_left = true;
	b->is_left = false;
	a->p = new_node;
	b->p = new_node;
	return new_node;
}

void get_clauses(node *root, vector<vector<string> >& cls){
	if(root == NULL){
		cout<<"there's a problem in get_clauses\n";
		return;
	}
	if(root->sym == OR_TOKEN){
		vector<node *> v;
		get_leaves(root,v);
		vector<string> s;
		for(int i = 0; i < v.size(); ++i ){
			string name = id_to_name[v[i]->id];
			if(v[i]->var_neg)
				name += "~";
			s.push_back(name);
		}
		cls.push_back(s);
		return;
	}
	else if(root->sym == VAR_TOKEN){
		string name = id_to_name[root->id];
		if(root->var_neg){
			name+= "~";
		}
		vector<string> s;
		s.push_back(name);
		cls.push_back(s);
		return;
	}
	else if(root->sym == AND_TOKEN){
		get_clauses(root->l,cls);
		get_clauses(root->r,cls);
	}
	else{
		cout<<"there's some problem in get_clauses"<<endl;
		return;
	}
}

node *dnf(node *root,stack<string> &rules){
	if(root->sym == OR_TOKEN){
		dnf(root->l, rules);
		dnf(root->r, rules);
	}
	else if(root->sym == AND_TOKEN){
		dnf(root->l, rules);
		dnf(root->r, rules);
		distribute(root, rules);
	}
	else if(root->sym == VAR_TOKEN){
		return root;
	}
	else{
		cout<<"there's some problem\n";
	}
}


int main(){
	in.open("test.txt");
	out.open("output.txt");
	string formula_string1, formula_string2;
	getline(in,formula_string1);
	node *f1 = change_to_formula_tree(formula_string1);
    string converted1 = tree_to_string(f1);
    stack<string> rules1;
   	string not_not_expanded = tree_to_string(f1);
   	not_expand(f1,handle_special_not_case(f1));
   	string not_expanded = tree_to_string(f1 = find_top_node(f1));
   	cout<<"before:\n";
   	cout<<not_not_expanded<<endl;
   	cout<<"After:\n";
   	cout<<not_expanded<<endl;
   	f1 = find_top_node(f1);
   	cout<<"distribute formula:\n";
   	dnf(f1,rules1);
   	cout<<tree_to_string(find_top_node(f1))<<endl;
   	f1 = find_top_node(f1);
   	not_expand(f1,true);
   	cout<<"negation:"<<endl;
   	cout<<tree_to_string(find_top_node(f1))<<endl;

   	vector<vector<string> > cls;
   	get_clauses(find_top_node(f1),cls);
   	cls = clean_formula(cls);
   	print_clauses(cls);
    resolution(cls);
    return 0;
}
