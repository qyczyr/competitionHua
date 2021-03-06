#include "route.h"
#include "lib_record.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <algorithm>
#include <iterator>
#include <vector>

#include <time.h>

using namespace std;

const int maxint = 999999;
const int max_demand_nodes = 50;
const int invalid_node = 1000;
const int no_pass = 0;


int split(char **dst,char *str, const char* spl)
{
	int n=0;
    char *result = NULL;
	result = strtok(str, spl);
	while(result != NULL)
	{
		strcpy(*dst++, result);
		result = strtok(NULL, spl);
		n++;
	}
	return n;
}	

void print_weights(int w[600][600],int vernum)
{
     for(int i=0; i<vernum; i++)
     {
		 for(int j=0 ;j<vernum; j++)
	     {
			printf("%d", w[i][j]);
	        printf("  ");	 
		 }
		 printf("\n");
      }
}

int create_graph(int weights_index[600][600], int weights[600][600], char *topo[5000], int edge_num)
{
    int vernum = 0;
    //update input weights
	int index, p, q, weight;
    const char *del = ",";
	char *r[4];
    for(int i=0; i<4; i++)
	{
		//bug
	   r[i] = new char[10];
	}

	int max_node = 0;
	for(int i=0; i<edge_num; i++)
	{
        split(r,topo[i], del);
		index = atoi(r[0]);
		p = atoi(r[1]);
		q = atoi(r[2]);
		weight = atoi(r[3]);
	    weights_index[p][q] = index;
		weights[p][q] = weight;
		if(p > max_node){max_node = p ;}
		if(q > max_node){max_node = q ;}
	}
	//printf("max_node: %d\n", max_node);
	vernum = max_node + 1;

	for(int i =0; i < 4; i++)
	{
		if(r[i])
		{
		   delete r[i];
		   r[i] = NULL;
		}
	}
	return vernum;
}

int  get_demand(int *s, int *t, int** include_nodes, char *demand)
{
	const char *del = ",";
	char **r = new char*[3];
	for(int i=0; i < 3; i++)
	{
		//there is bug
		r[i] = new char[200];
	}		
	split(r, demand, del);
    *s = atoi(r[0]);
	*t = atoi(r[1]);

    const char *temp = "|";

	char **demand_nodes = new char*[max_demand_nodes];
    for(int i=0; i < max_demand_nodes; i++)
	{
		demand_nodes[i] = new char;
	}
	int n = split(demand_nodes , r[2], temp);
	*include_nodes = new int[n];
	for(int i=0; i<n; i++)
	{
	    *(*(include_nodes)+i) = atoi(demand_nodes[i]);
    }
	//free r memory
	for(int i=0; i < 3; i++)
	{
		if(r[i])
		{
			delete r[i];
			r[i] = NULL;
		}
	}
    delete r;
    //free demand_nodes memory
	for(int i=0; i < max_demand_nodes; i++)
	{
		if(demand_nodes[i])
		{
			delete demand_nodes[i];
			demand_nodes[i] = NULL;
		}
	}
    delete demand_nodes;
	return n;

}




void dijkstra(int startpoint, int vernum, int edge_num,int *dist, int *prev, int w[600][600], bool s[600]) 
{
	//bool s[5000];
	for(int i=0; i<vernum; i++)
	{
        dist[i] = w[startpoint][i];
		s[i] = 0;
		if(dist[i] == maxint)
			prev[i] = invalid_node;
		else
			prev[i] = startpoint;//record pre point
	}
    
	dist[startpoint] = 0;
	s[startpoint] = 1;
	for(int i=1; i<vernum; ++i)
	{
		int tmp = maxint;
		int u = startpoint;
		for(int j=0; j<vernum; ++j)
		{
			if((!s[j]) && (dist[j] < tmp))
			{
				u = j;
				tmp = dist[j];
			}
		}
		s[u] = 1;
 
       for(int j=0; j<vernum; j++)
	   {
			if((!s[j]) && w[u][j] < maxint) 
			{
				int newdist = dist[u] + w[u][j];
                if(newdist < dist[j])
				{
					dist[j] = newdist;
					prev[j] = u;
				}
			}
	   }
		
	}
}

void searchPath(int *prev, int vernum, int s, int t, vector<int> *result)
{
	int que[5000];
	int tot = 1;
	que[0] = t;
	int tmp = prev[t];
	bool isPass = true;
	for(int i = 0; i < vernum - 1; i++)
	{
		if(tmp == s) break;
	
		que[tot] = tmp;
		tmp = prev[tmp];
        /*
		if(que[tot] == tmp)
		{
			isPass = false;
			break;
        }*/
		tot++;
		
	}

	que[tot] = s;
	if(isPass)
	{	
		for(int i = tot; i> 0; i--)
		{
			result->push_back(que[i]);
		}
	}
}

void BFS(int *map, int *path, int vernum)
{
	bool visited[vernum];
	queue<int> qu, qq;
	int s, t;
	while(true)
	{
		s = qu.size();
		if(!s) return;
		while(s--)
		{
			t = qu.front();
			for(int i = 0; i < vernum; i++)
			{
				if(*(map + vernum * t + i) != maxint &&  !visited[i])
				{
					visited[i] = 1;
					qu.push(i);
					path[i] = t;
				}
				
			}
			qu.pop();
		}
	
	}

}


void full_array(int *array, int l, vector<int*> *result)
{
	do
	{
		int *perm_result = new int[l];
		copy(array, array+l, perm_result);
       	result->push_back(perm_result);
	
	}while(next_permutation(array, array+l));

}




bool searchIncludeNodesPath(int *prev, int vernum, int s, int t, int *include_nodes, int include_nodes_num, vector<int> *result)
{
	int que[5000];
	int tot = 1;
	que[0] = t;
	int tmp = prev[t];
	bool isPass = true;
	for(int i = 0; i < vernum - 1; i++)
	{
	 	if(tmp == s) break;
	    
		que[tot] = tmp;
		tmp = prev[tmp];
        
		/*
        if(que[tot] == tmp)
		{
			isPass = false;
			break;
        }*/
		tot++;
		
	}

	que[tot] = s;

	bool bIsUse = true;
	if(isPass)
	{	
		//bool bIsUse = true;
		for(int i = tot - 1; i> 0; i--)
		{
            for(int j = 0; j < include_nodes_num; j++)
			{
				if(que[i] == include_nodes[j]) 
				{	
					bIsUse = false;
					break;
				}

			}
			if(!bIsUse) break;
        }
		if(bIsUse)
		{
			for(int i=tot; i > 0; i-- )
			{
				result->push_back(que[i]);
			}
		}
	}
	return bIsUse;
}

int get_result(vector<vector<int> > spath, int *sdist, vector<vector<int> > vpath,  map<int,int*> include_nodes_dist, vector<vector<int> > tpath, vector<vector<int> > *pass_path, int *include_nodes, int include_nodes_num)
{

	int shortest_dist =0;
    vector<int> result(0);

	for(unsigned int i=0; i < spath.size(); i++)
	{
		for(unsigned int j=0; j < vpath.size(); j++)
		{
			if(spath[i].back() == vpath[j].front())
			{
				for(unsigned int k=0; k < tpath.size(); k++)
				{
		           if(vpath[j].back() == tpath[k].front())
				   {
					   
	     			   bool isAgainFlag = false;
				       for(vector<int>::iterator iter = spath[i].begin(); iter != spath[i].end() -1; iter++)
					   {
		    				if(find(vpath[j].begin(), vpath[j].end(), *iter ) != vpath[j].end())
							{	
								isAgainFlag = true;

							}
						
					   }
                       if(isAgainFlag)
					   {
							break;
					   }
					   for(vector<int>::iterator iter = vpath[j].begin(); iter != vpath[j].end() -1; iter++)
					   {
					         if(find(tpath[k].begin(), tpath[k].end(), *iter) != tpath[k].end())
							 {	
								 isAgainFlag = true;
							 }
					   
					   }
					   if(isAgainFlag) break;
                       
					   int s_v_node = spath[i].back(); 
					   int v_v_node = vpath[j].back();
					   int v_t_node = tpath[k].back(); 
					
					   int v_v_dist = 0;
                       //cout << "there is a problem" <<endl;
					   /*there is problem*/
					   int g = 0;

					   for(unsigned int m = 1; m < vpath[j].size(); m++)
					   { 
							int key =  vpath[j][m];
							for(int h = 0; h < include_nodes_num; h++)
							{
                               if(key == include_nodes[h])
								{
									//cout << "vpath:" << vpath[j][g] << endl;
									int *tmp_dist = include_nodes_dist[vpath[j][g]];
									//cout << "vpath_m:" << vpath[j][m] <<endl;
									//cout << "tmp_dist: " << tmp_dist[vpath[j][m]]<< endl;
									v_v_dist += tmp_dist[vpath[j][m]];
									g = m;
								}
							}
					   }
                       
                       bool is_saved_path = false;
                       int path_dist =  sdist[s_v_node] + v_v_dist + include_nodes_dist[v_v_node][v_t_node];
                       //cout << "path:" << sdist[s_v_node]<< "  v-v:  "  << v_v_dist<< " v-t "<<include_nodes_dist[v_v_node][v_t_node] <<endl;
					   if(0 == shortest_dist) 
					   {
						   shortest_dist = path_dist;
					       is_saved_path = true;
					   }
                       if(path_dist < shortest_dist)
					   { 
                            shortest_dist = path_dist;
							is_saved_path = true;
					   }
					   if(is_saved_path)
					   {
						    result.clear();
							result.insert(result.end(), spath[i].begin(), spath[i].end()-1);
							result.insert(result.end(), vpath[j].begin(), vpath[j].end()-1);
							result.insert(result.end(), tpath[k].begin(), tpath[k].end());
						}
                       
                       /*                      
					   vector<int> result(0);
					   result.insert(result.end(), spath[i].begin(), spath[i].end()-1);
					   result.insert(result.end(), vpath[j].begin(), vpath[j].end()-1);
					   result.insert(result.end(), tpath[k].begin(), tpath[k].end());
                       pass_path->push_back(result);
					   */
				   }
			     
				}
			}
		
		}
	
	}
	pass_path->push_back(result);
	return shortest_dist;

}


//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
    cout << "demand" << demand <<endl; 
	cout << "hello" <<endl;
    int vernum = 0;   // number of vertex
	int weights[600][600];  //weights between nodes
	int weights_index[600][600]; //nodes index
    
    int include_nodes_weights[600][600];
	for(int i=0; i < 600; i++)
	{
		for(int j=0; j < 600; j++)
		{
		    weights[i][j] = maxint;
			weights_index[i][j] = maxint;
			include_nodes_weights[i][j] = 0;
	    }	
	}

	int s_p = 0, t_p = 0; // s---> t
	int *include_nodes = NULL;   //must include nodes
    printf("number of edge: %d\n", edge_num);
	int include_nodes_num = get_demand(&s_p, &t_p, &include_nodes, demand);
    printf("s:%d\n", s_p);
	printf("t:%d\n", t_p);
	printf("number of must include nodes: %d\n",include_nodes_num);
	printf("The must include nodes: ");
	for(int i=0; i< include_nodes_num; i++)
		printf("%d | ", include_nodes[i]);
    printf("\n");	
	//get weights from topo
    vernum = create_graph(weights_index, weights, topo, edge_num);
    printf("number of vertexs: %d\n", vernum);
    //printf weights
	//printf("weights:\n");
	//print_weights(weights, vernum);
	//printf("index of nodes:\n");
    //print_weights(weights_index, vernum);
	

	//full array between must include nodes
    vector<int*> include_nodes_farr;

	full_array(include_nodes, include_nodes_num, &include_nodes_farr);
    /*	
    cout << "the number of full array: " << include_nodes_farr.size() << endl; 
	for(unsigned int i = 0; i < include_nodes_farr.size(); i++)
	{
		for(int j=0; j < include_nodes_num; j++)
		{
			cout << include_nodes_farr[i][j] << " ";
		}
		cout << endl;
	
	}*/

    //s -- > V'
	//
	
	bool s[600];
    //memset(s, 0, 600);    
    int *sDist = new int[vernum];
    //memset(sDist, maxint, vernum);

	int *sPrev = new int[vernum];
    //sPrev[s_p] = invalid_node; //===================================

	time_t s_start_time = time(NULL);
    
    dijkstra(s_p, vernum, edge_num, sDist, sPrev, weights, s);
    /*
    printf("dist: ");
	for(int i=0; i < vernum; i++)
		printf("%d  ", sDist[i]);
    printf("\n"); 
    cout << "s->v path:" <<endl;
    */ 
    vector<int> eachpath;
 	//cout << "eachpath:" << eachpath.size() <<endl;
    vector<vector<int> > sToinclude_nodes_path(include_nodes_num, eachpath);

    for(int i=0; i < include_nodes_num; i++)
	{
        //cout << endl;        
        //cout << "s to include nodesz; " << s_p << " -> " << include_nodes[i] << endl;
		//cout << "dist" << sDist[include_nodes[i]] << endl;
        //if(sDist[include_nodes[i]] != maxint )
		//{	

		searchPath( sPrev, vernum, s_p, include_nodes[i], &sToinclude_nodes_path[i]);
		sToinclude_nodes_path[i].push_back(include_nodes[i]);
		//}
	}
	
	time_t s_end_time = time(NULL);
	cout << "s cost time: " << s_end_time - s_start_time << endl;
   

	//
	//
	//path between must include nodes
    
	//cout << endl;
    //cout << "include nodes------------------------------------------- include nodes"<<endl;
	//
    time_t 	v_start_time = time(NULL);

    map<int, int*> include_nodes_dist;

	map<int, int*> include_nodes_prev;
    //vector<int*> include_nodes_dist;
	//vector<int*> include_nodes_prev;

	for(int i=0; i < include_nodes_num; i++)
	{
		int *tmp_dist = new int[vernum];
		//memset(tmp_dist, maxint, vernum);
		int *tmp_prev = new int[vernum];
		//tmp_prev[include_nodes[i]] = invalid_node;
		bool bV[600];
		//memset(bV, 0, 600);

		dijkstra(include_nodes[i], vernum, edge_num, tmp_dist, tmp_prev, weights, bV);
		//include_nodes_dist.push_back(tmp_dist);
		//include_nodes_prev.push_back(tmp_prev);
		
		include_nodes_dist.insert(pair<int,int*>(include_nodes[i], tmp_dist));
		include_nodes_prev.insert(pair<int, int*>(include_nodes[i], tmp_prev));

	}
   time_t v_dij_time = time(NULL);
   cout << "v_dij_cost_time: " << v_dij_time - v_start_time << endl;

 
   time_t v_start_search_time = time(NULL);
   
	//vector<int> tmp;
    //vector<vector<int> > include_nodes_path(include_nodes_farr.size(), tmp); 
    vector<vector<int> > include_nodes_path; 
    
    for(unsigned int i=0 ; i < include_nodes_farr.size(); i++)
	{
		 vector<int> each_arr_result_path;
         for(int j=0; j < include_nodes_num - 1; j++)
		 {
             int from_s = include_nodes_farr[i][j];
             int to_t = include_nodes_farr[i][j+1];

			 if(include_nodes_weights[from_s][to_t]  == maxint)
			 {
				 //cout << "don't arrived!" << endl;
				 each_arr_result_path.clear();
				 break;
             }

		     bool isPass = searchIncludeNodesPath(include_nodes_prev[include_nodes_farr[i][j]], vernum, include_nodes_farr[i][j], include_nodes_farr[i][j+1], include_nodes_farr[i], include_nodes_num, &each_arr_result_path);
             
			 //cout << "isPass: " << isPass <<endl;
             
			 if(!isPass)
			 {
				 int temp_s = include_nodes_farr[i][j];
				 int temp_t = include_nodes_farr[i][j+1];
				 //cout << "temp_s: " << temp_s << "  " << "temp_t: " << temp_t <<endl;
				 include_nodes_weights[temp_s][temp_t] = maxint;
				 each_arr_result_path.clear();
				 break;
			 }
		 }
         if(each_arr_result_path.size() > 0)
	     {
			cout << "each_arr_result_path: " << each_arr_result_path.size() <<endl;
			each_arr_result_path.push_back(include_nodes_farr[i][include_nodes_num -1]);
			include_nodes_path.push_back(each_arr_result_path);
		 }
    }
    
    /*
    for(unsigned int i=0; i < include_nodes_path.size(); i++)
	{
	  for(unsigned int j = 0; j < include_nodes_path[i].size(); j++)
	  {
	      cout << include_nodes_path[i][j] << " ";
	  }
	  cout << endl;
	
	}*/
  




   time_t v_end_search_time = time(NULL);
   cout << "v_search_cost_time: " << v_end_search_time - v_start_search_time << endl;

   
   /*
   cout << "-------------------------V clean path:--------------------------------" <<endl;
   vector<vector<int> > clean_include_nodes_path;
   for(unsigned int i=0; i < include_nodes_path.size(); i++)
   {

	int count = 0;
	for(int j = 0; j < include_nodes_num; j++)
     {
        
		vector<int>::iterator result = find(include_nodes_path[i].begin(), include_nodes_path[i].end(), include_nodes[j]);
		if(result != include_nodes_path[i].end())
		{
		      count ++;	
		}
		
	 }
     if(count == include_nodes_num)
	 {
		clean_include_nodes_path.push_back(include_nodes_path[i]);
	 }
			
   }*/
   

   //------------------------V'-->t---------------------------
   
   
    vector<int> VtPath;
    vector<vector<int> > include_nodesTot_path(include_nodes_num, VtPath);
	int i =0;
	for(map<int, int*>::iterator iter = include_nodes_prev.begin(); iter != include_nodes_prev.end(); iter++ )
	{
		searchPath( iter->second , vernum,  iter->first, t_p, &include_nodesTot_path[i]);
		include_nodesTot_path[i].push_back(t_p);
		i++;
    }

    

   // cout << "------------------S->V--------------- path" <<endl;
   // print_path(sToinclude_nodes_path);
	
	//cout << "-------------------V-V path--------------" <<endl;
	//print_path(clean_include_nodes_path);

	//cout << "----------------V to Dist------------------------" <<endl;
	//print_path(include_nodesTot_path);
	
   
	vector<vector<int> > pass_path;

    int shortest_path = get_result(sToinclude_nodes_path, sDist, include_nodes_path, include_nodes_dist, include_nodesTot_path, &pass_path, include_nodes, include_nodes_num);
    
    cout <<  "shortest_path: " << shortest_path <<endl;

        
	print_path(pass_path);


	convert_demand_path(pass_path, weights_index);
	 

   //time_t t_search_end_time = time(NULL);
   //cout << "t__search_cost_time: " << t_search_time - t_search_end_time << endl;


 
}


void print_path(vector<vector<int> > path)
{
	for(unsigned int i=0; i < path.size(); i++)
	{
		for(unsigned int j=0; j < path[i].size(); j++)
		{
			cout << path[i][j] << "  ";
	
		}
		cout << endl;
	}

}

void convert_demand_path(vector<vector<int> > path, int weights_index[600][600])
{
	for(unsigned int i= 0; i < path.size(); i++)
	{
		for(unsigned int j=0; j < path[i].size()-1; j++)
		{
			int p = path[i][j];
			int q = path[i][j+1];
			cout << weights_index[p][q] << " | ";
		}
		cout << endl;
	}

}







