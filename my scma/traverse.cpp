#include "traverse.h"
#include <future>
#include <chrono>
using namespace std;

vector<vocab_value_t> traverse::firstEvaluation()
{
	reset();

	//mohammad
	if(satcount(*my_bdd, my_nodes[0])!=0) {return my_oneSAT(*my_bdd, main_node);}
	else{
		return ;
	}
}
void traverse::reset()
{
	depth = 0;
	int i = 0;
	while (i < size)
	{
		choice[i] = vuu;
		i++;
	}
	res = false;
	allTraversed = false;

	//mohammad
	bdd_free(*my_bdd);
	*my_bdd=bdd_new(size);
	for(int i=0;i<size;i++){
		my_nodes.push_back(make(*my_bdd, i, 0, 1));
	}
}
traverse::traverse()
{
}

traverse::traverse(SpecCheckVocab &scv)
{
	depth = 0;
	int i = 0;

	SCV = &scv;

	size = SCV->vocab.size();

	while (i < size)
	{
		choice.push_back(vuu);
		i++;
	}
	res = false;
	allTraversed = false;

	//mohammad
	//creating bdd
	*my_bdd=bdd_new(size);
	for(int i=0;i<size;i++){
		my_nodes.push_back(make(*my_bdd, i, 0, 1));
	}
}

void traverse::setTraverse(SpecCheckVocab &scv)
{
	depth = 0;

	SCV = &scv;
	size = SCV->vocab.size();
}

vector<vocab_value_t> traverse::getEvaluation()
{
	//return choice;
	return my_oneSAT(*my_bdd, my_nodes[0]);
}
// mohammad 	
/* my_onesat(B, u) returns one satisfying assignment for node u in B */
vector<vocab_value_t>traverse::my_oneSAT(bdd B, bdd_node u){

	REQUIRES(is_bdd(B));
	assert(0 <= u && u < B->size);
	assert(u != 0);		/* bdd not satisfiable! */
	vector<vocab_value_t> my_choice;
	if (u == 0)
		printf("No solutions\n");
	else {
		node a = B->T[u];
		int v = a->var;
		while (v <= B->num_vars) {
			//printf("x[%d]=", v);
			if (a->low != 0) {
			//printf("0\n");
				my_choice.push_back(vff);
				u = a->low;
			} 
			else {
				//printf("1\n");
				my_choice.push_back(vtt);
				u = a->high;
			}
			a = B->T[u];
			v = a->var;
		}
	}
	return my_choice; 
}

vector<vocab_value_t> traverse::nextEvaluation()
{
	/*while (true)
	{
		//cannot put res check (matchesUnsatCore) here, since if depth=size the choice will be returned
		//+ will repeat check for same choice when coming back to the choice by traverse up. however below, 
		//the check is done only on change of choice everywhere other than in traverseup
		if (choice[depth] == vtt)
		{
			choice[depth] = vff;
			res = SCV->matchesUnsatCore(choice);
			if (res == true)
			{
				cout << "SPCHK: Ignore: subtree satisfies an eliminated pattern.";
				print_vector(choice);
				//no left-side branch @ same level(choice[depth]=vff), move up
				traverseUp(); //after traverse up function, check allTraversed
				if (allTraversed == true)
				{
					cout << "done" << endl;
					print_vector(choice);
					return choice;
				}
			}
			else
				depth++;
		}

		else if (choice[depth] == vff)
		{
			traverseUp();
			if (allTraversed == true)
			{
				cout << "done" << endl;
				print_vector(choice);
				return choice;
			}
		}
		else if (choice[depth] == vuu)
		{
			choice[depth] = vtt;
			res = SCV->matchesUnsatCore(choice);
			if (res == true)
			{
				cout << "SPCHK: Ignore: subtree satisfies an eliminated pattern.";
				print_vector(choice);
				//move to left-side branch @ same level
				//note that reaching current level means parent not in unsat core, which means at least left or right branch 			not in unsat core. if right branch is in unsat core (res=true) => right is not. no need to check again
				choice[depth] = vff;
				depth++;
			}
			else
				depth++;
		}

		if (depth == choice.size())
		{
			depth--;
			//	if(allTraversed==true){ cout<<"All vocab evaluations are covered. Evaluations will restrart.\n"<<endl;}
			cout << "choice set to: ";
			print_vector(choice);
			return choice;
		}
	}*/

	//remove last used assignment
	bdd_node last_assign=1;
	bdd_node my_x;
	for(int i=0;i<current_assignment.size();i++){
		my_x=make(*my_bdd, i, (int)!current_assignment[i], (int)current_assignment[i]); //transform current assignment to node
		last_assign=apply(*my_bdd,&and, last_assign, my_x);
	}
	main_node=apply(*my_bdd,&and, main_node,  apply(*my_bdd, &xor, last_assign, 1)); // f & (~last assignment)

	/* get an assignment*/
	//adding a timeout
	std::cout << "waiting to get new assignment...\n";
    std::future_status status;
	std::future<int> future = std::async(std::launch::async, [](){ 
        	current_assignment=my_oneSAT(*my_bdd, main_node);
        	return 1;  
    	});
	if(satcount(*my_bdd, my_nodes[0])!=0) {
		do {
			status = future.wait_for(std::chrono::minutes(10));
			if (status == std::future_status::deferred) {
				std::cout << "deferred\n";
			} else if (status == std::future_status::timeout) {
				std::cout << "timeout \n returning to use old traversal ";
			} else if (status == std::future_status::ready) {
				std::cout << "ready!\n";
			}
    	} while (status != std::future_status::ready); 
		return current_assignment;
		
		
	}

	
}

void traverse::traverseUp()
{
	choice[depth] = vuu;
	depth--;
	if (depth == -1)
	{
		allTraversed = true;
		depth = 0;
	}
}

/*
traverse::traverse()
{
size=3;
depth=0;
int i=0;
while(i<size)
	{
	choice.push_back(vuu);
	i++;
	}
res=false;
allTraversed=false;

}

traverse::traverse(int Size)
{
size=Size;
depth=0;
int i=0;
while(i<size)
	{
	choice.push_back(vuu);
	i++;
	}
res=false;
allTraversed=false;

}
*/
