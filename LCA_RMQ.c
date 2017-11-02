#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#define MAX 1000 
#define max(a,b) ( (a>b)?a:b )
#define min(a,b) ( (a<b)?a:b )

int euler[2*MAX - 1]; // Euler Tour Array ( DFS Traversal in a Tree )
int firstOccur[MAX+1];	// To Store the idx of first occuurence of the node in Euler Tour
int level[MAX+1];
bool tree[MAX+1][MAX+1];
int idx,nodes;
int segtree[2*MAX - 1];

void initialize(){
	int i;
	for(i=0;i<=(2*MAX-2);i++){
		if(i<=MAX)	
			firstOccur[i]=-1;
		euler[i]=0,segtree[i]=0;
	}
}

void build(int node,int start,int end){
	int mid;
	if(start==end)
		segtree[node]=euler[start];
	else {
		mid=(start+end)/2;
		build(2*node,start,mid); // left sub Segment tree
		build(2*node+1,mid+1,end);	// right sub Segment tree
		int x=segtree[2*node];
		int y=segtree[2*node+1];
		segtree[node]=(x>y)?y:x;
	}
}

int query (int node,int start,int end, int l, int r){
	int mid,x,y;
	if(r<start || end<l)
		return INT_MAX;
	if(l<=start && end<=r)
		return segtree[node];
	mid=(start+end)/2;
	x=query(2*node,start,mid,l,r);
	y=query(2*node+1,mid+1,end,l,r);
	return ((x>y)?y:x);
}

void dfs_eulertour(int current,int previous,int l)
{
	if(current<=nodes)
	{
		if(firstOccur[current]==-1)
			firstOccur[current]=idx;
		euler[idx++]=current;
		int j;
		for(j=0;j<=MAX;j++)
		{
			if(tree[current][j] && j!=previous)
			{
				dfs_eulertour(j,current,l+1);
				euler[idx++]=current;
			}
		}
	}
}

int findLCA(int u,int v){
	int i;
	idx=0;
	dfs_eulertour(1,0,0);
	printf("Euler-Tour Array : ");
	for(i=0;i<(2*nodes-1);i++){
		printf("%d ",euler[i]);
	}
	printf("\n");
	build(1,0,2*nodes-2); // Building Segment Tree from Euler Tour
	return query(1,0,2*nodes-2,min(firstOccur[u],firstOccur[v]),max(firstOccur[v],firstOccur[u]));
}

int main() {
	int i,t,u,v;
	printf("Enter the number of nodes in the Tree : ");
	scanf("%d",&nodes);
	printf("Enter the Edges in Pairs U V : \n");
	for(i=0;i<nodes-1;i++)	scanf("%d%d",&u,&v), tree[u][v]=true;
	//findLCA(1,nodes);
	printf("Enter the Number of LCA Queries : ");
	scanf("%d",&t);
	while(t--){
		initialize();
		printf("Find LCA of two nodes (U,V) : ");
		scanf("%d%d",&u,&v);
		printf("%d\n",findLCA(u,v));
	}
	return 0;
}
