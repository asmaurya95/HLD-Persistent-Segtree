/* Project by : 

Ashutosh Maurya ( CSE + OSS , Semester V) ,. 

Minor-1*/

/* C Program for Heavy-Light Decomposition of a tree with Roll Back Operation using Persistent Segment Trees */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define max(a,b) (a>b)?a:b
#define ll long long int 
#define N 1001

int i,j; 
int tree[N][N];  // Matrix representing the tree


/* a tree node structure. Every node has a parent, depth,
   subtree size, chain to which it belongs and a position
   in base array*/
struct treeNode
{
    int par;   // Parent of this node
    int depth; // Depth of this node
    int size; // Size of subtree rooted with this node
    int pos_segbase; // Position in segment tree base
    int chain;
} node[N];
 
/* every Edge has a weight and two ends. We store deeper end */
struct Edge
{
    int weight;  // Weight of Edge
    int deeper_end; // Deeper end
} edge[N];

// Base Array of Edge Weights on which the segment tree is to be constructed
int base_array[N];

// data type for individual node in the segment tree
struct seg_node
{
    int val;
    struct seg_node *left, *right;
};

struct seg_node* version[N];
int curr_ver, next_ver=1;

// C Style Function Constructor for node
struct seg_node* node_new(struct seg_node* l,struct seg_node* r,int v)
{
    struct seg_node* p=malloc(sizeof(struct seg_node));
    p->left=l;
    p->right=r;
    p->val=v;
    return p;
}

/* Function declarations */
// A function to add Edges to the Tree matrix
void addEdge(int,int,int,int);
// A recursive function for DFS on the tree
void dfs(int,int,int,int);
// A recursive function that decomposes the Tree into chains i.e the prime Heavy Light Decomposition function
void hld(int,int,int*,int*,int,int[]);
// A recursive function that constructs the Segment Tree using edge weights as elements for the segment tree base array
int construct_ST(int,int,struct seg_node*);
// A recursive function that updates the Segment Tree and Creates a New Version of Segment Tree, retaining the previous version
void update_ST(struct seg_node*,struct seg_node*,int,int,int,int);
// A function to update an Edge's value to a new value in segment tree
void change(struct seg_node*,struct seg_node*,int,int,int);
// A function to find the Lowest Common Ancestor of given two nodes in a tree
int LCA(int,int,int);
// A Function to return the Maximum value in a given range 
int RMQUtil(struct seg_node*,int,int,int,int);
// A function to process Range Maximum Queries, utlizes the RMQUtil Function
int RMQ(struct seg_node*,int,int,int);
// A function to move from one node to another while keeping track of the maximum
int crawl_tree(struct seg_node*,int,int,int,int[]);
// A function to process Maximum Edge Queries
void maxEdge(struct seg_node*,int,int,int,int[]);
// A Function to read all tree information from the user
void readInput(int*);
// A function to input and process queries from the user
void processQueries(int,int[]);

// e is Edge ID, u and v are the two nodes, w is weight
void addEdge(int e, int u, int v, int w)
{
    /*tree as undirected graph*/
    tree[u-1][v-1] = e-1;
    tree[v-1][u-1] = e-1;
 
    edge[e-1].weight = w;
}

// A function to get the LCA of nodes u and v
int LCA(int u, int v, int n)
{
    /* array for storing path from u to root */
    int LCA_aux[n+5];
 
    // Set u is deeper node if it is not
    if (node[u].depth < node[v].depth)
    {
        int temp=u;
        u=v;
        v=temp;
    }
 
    /* LCA_aux will store path from node u to the root*/
    memset(LCA_aux, -1, sizeof(LCA_aux));
 
    while (u!=-1)
    {
        LCA_aux[u] = 1;
        u = node[u].par;
    }
 
    /* find first node common in path from v to root and u to
       root using LCA_aux */
    while (v)
    {
        if (LCA_aux[v]==1)break;
        v = node[v].par;
    }
 
    return v;
}

 
/*  A recursive function to get the minimum value in a given range
     of array indexes. The following are parameters for this function.
    st    --> Pointer to segment tree
    index --> Index of current node in the segment tree. Initially
              0 is passed as root is always at index 0
    ss & se  --> Starting and ending indexes of the segment represented
                  by current node, i.e., st[index]
    qs & qe  --> Starting and ending indexes of query range */
int RMQUtil(struct seg_node* n,int ss, int se, int qs, int qe)
{
    //printf("%d,%d,%d,%d,%d\n", ss, se, qs, qe, index);
 
    // If segment of this node is a part of given range, then return
    //  the max of the segment
    if (qs <= ss && qe >= se-1){
        //printf("%d\n",n->val);
        return n->val;
    }
 
    // If segment of this node is outside the given range
    if (se-1 < qs || ss > qe)
        return -1;
 
    // If a part of this segment overlaps with the given range
    int mid = (ss + se)/2;
    
    return max(RMQUtil(n->left, ss, mid, qs, qe), 
                RMQUtil(n->right, mid, se, qs, qe));
}
 
// Return minimum of elements in range from index qs (quey start) to
// qe (query end).  It mainly uses RMQUtil()
int RMQ(struct seg_node* n1,int qs, int qe, int n)
{
    // Check for erroneous input values
    if (qs < 0 || qe > n-1 || qs > qe)
    {
        printf("Invalid Input");
        return -1;
    }
 
    return RMQUtil(n1, 0, n, qs, qe);
    //printf("yes\n");
}

// we move to the surface changing u and chains
// until u and v donot belong to the same
int crawl_tree(struct seg_node* curr,int u, int v, int n, int chain_heads[])
{
    int chain_u, chain_v = node[v].chain, ans = 0;
 
    while (1)
    {
        chain_u = node[u].chain;
 
        /* if the two nodes belong to same chain,
         * we can query between their positions in the array
         * acting as base to the segment tree. After the RMQ,
         * we can break out as we have no where further to go */
        if (chain_u==chain_v)
        {
            if (u==v);   //trivial
            else
              ans = max(RMQ(curr,node[v].pos_segbase+1, node[u].pos_segbase, n),
                        ans);
            break;
        }
 
        /* else, we query between node u and head of the chain to which
           u belongs and later change u to parent of head of the chain
           to which u belongs indicating change of chain */
        else
        {
            ans = max(ans,
                      RMQ(curr,node[chain_heads[chain_u]].pos_segbase,
                          node[u].pos_segbase, n));
 
            u = node[chain_heads[chain_u]].par;
        }
    }
 
    return ans;
}

// curr is the current node, prev is the parent of curr,
// dep is its depth
void dfs(int curr, int prev, int dep, int n)
{
    /* set parent of current node to predecessor*/
    node[curr].par = prev;
    node[curr].depth = dep;
    node[curr].size = 1;
 
    /* for node's every child */
    for (int j=0; j<n; j++)
    {
        if (j!=curr && j!=node[curr].par && tree[curr][j]!=-1)
        {
            /* set deeper end of the Edge as this child*/
            edge[tree[curr][j]].deeper_end = j;
 
            /* do a DFS on subtree */
            dfs(j, curr, dep+1, n);
 
            /* update subtree size */
            node[curr].size+=node[j].size;
        }
     }
}
 

void hld(int curr_node, int id, int *edge_counted, int *curr_chain,
         int n, int chain_heads[])
{
    /* if the current chain has no head, this node is the first node
     * and also chain head */
    if (chain_heads[*curr_chain]==-1)
        chain_heads[*curr_chain] = curr_node;
 
    /* set chain ID to which the node belongs */
    node[curr_node].chain = *curr_chain;
 
    /* set position of node in the array acting as the base to
       the segment tree */
    node[curr_node].pos_segbase = *edge_counted;
 
    /* update array which is the base to the segment tree */
    base_array[(*edge_counted)++] = edge[id].weight;
 
    /* Find the special child (child with maximum size)  */
    int spcl_chld = -1, spcl_edg_id;
    for (int j=0; j<n; j++)
      if (j!=curr_node && j!=node[curr_node].par && tree[curr_node][j]!=-1)
        if (spcl_chld==-1 || node[spcl_chld].size < node[j].size)
           spcl_chld = j, spcl_edg_id = tree[curr_node][j];
 
    /* if special child found, extend chain */
    if (spcl_chld!=-1)
      hld(spcl_chld, spcl_edg_id, edge_counted, curr_chain, n, chain_heads);
 
    /* for every other (normal) child, do HLD on child subtree as separate
       chain*/
    for (int j=0; j<n; j++)
    {
      if (j!=curr_node && j!=node[curr_node].par &&
            j!=spcl_chld && tree[curr_node][j]!=-1)
      {
        (*curr_chain)++;
        hld(j, tree[curr_node][j], edge_counted, curr_chain, n, chain_heads);
      }
    }
}
 
// A recursive function that constructs Segment Tree-Version 0 for array[ss..se).
// si is index of current node in segment tree st
int construct_ST(int ss, int se, struct seg_node* n)
{
    // If there is one element in array, store it in current node of
    // segment tree and return
    //if(ss>(se-1))
        //return ;
    if (ss == se-1)
    {
        //s.tree[si] = base_array[ss];
        n->val=base_array[ss];
        return base_array[ss];
    }
 
    // If there are more than one elements, then recur for left and
    // right subtrees and store the minimum of two values in this node
    int mid = (ss + se)/2;
    //s.tree[si] =  max(construct_ST(ss, mid, si*2), construct_ST(mid, se, si*2+1));
    //return s.tree[si];
    n->left=node_new(NULL,NULL,0);
    n->right=node_new(NULL,NULL,0);
    n->val=max(construct_ST(ss,mid,n->left),construct_ST(mid,se,n->right));
    return n->val;
}
 
// idx is the node to be updated to value val
// si is the starting index of the segment tree
//  ss, se mark the corners of the range represented by si
void update_ST(struct seg_node* prev, struct seg_node* cur,int ss, int se,int idx, int value)
{
 
    if(idx>=se || idx<ss)
        return;
 
    else if(ss == se-1 && ss==idx)
    {
        cur->val=value;
        return;
    }
    else{
        int mid=(ss+se)/2;
        //int l_max,r_max;
        if(idx<mid)
        {
            cur->right = prev->right;
            cur->left=node_new(NULL,NULL,0);
            update_ST(prev->left,cur->left,ss,mid,idx,value);
        }
        else
        {
            cur->left=prev->left;
            cur->right=node_new(NULL,NULL,0);
            update_ST(prev->right,cur->right,mid,se,idx,value);
        }
        cur->val=max(cur->left->val,cur->right->val);
    }
}
 
void change(struct seg_node* prev,struct seg_node* cur,int e, int val, int n)
{
    update_ST(prev,cur,0, n, node[edge[e].deeper_end].pos_segbase, val);
}

// A function for MAX_EDGE query
void maxEdge(struct seg_node* curr,int u, int v, int n, int chain_heads[])
{
    int lca = LCA(u, v, n);
    int ans = max(crawl_tree(curr,u, lca, n, chain_heads), crawl_tree(curr,v, lca, n, chain_heads));
    printf("%d\n", ans);
}

void readInput(int *n)
{
    int x,y,wt;
    printf("Enter the no. of nodes :");
    scanf("%d",n);
    printf("Enter the pair of nodes along with the weight of edge between them:\n");
    for(i=1;i<(*n);i++)    scanf("%d%d%d",&x,&y,&wt), addEdge(i,x,y,wt);
} 

void processQueries(int n,int chain_heads[])
{
    int Q;
    printf("Query Options: \n1 X Y : Maximum of all Edge Weights between nodes X,Y\n2 E VAL: Change Value of Edge E to VAL\n3 X: Roll Back to the state after Xth Change \n");
    printf("Enter Number of Queries: ");
    scanf("%d",&Q);
    while(Q--)
    {
        int ch;
        int X,Y,E,VAL;
        scanf("%d",&ch);
        if(ch==1){
            scanf("%d%d",&X,&Y);
            maxEdge(version[curr_ver],X-1,Y-1,n,chain_heads);
        }
        else if(ch==2){
            scanf("%d%d",&E,&VAL);
            version[next_ver]=node_new(NULL,NULL,0);
            change(version[curr_ver],version[next_ver],E-1,VAL,n);
            curr_ver=next_ver;
            next_ver++;
        }
        else if(ch==3){
            scanf("%d",&X);
            curr_ver=X;
            printf("Now Accepting Queries corresponding to Tree Version %d\n",X);
        }
    }
}

int main()
{

    //FILE *fp;
    //fp=freopen("input.in","r",stdin);   // Input redirected from an input file containing the Test Cases
    /* fill adjacency matrix with -1 to indicate no connections */
    memset(tree, -1, sizeof(tree));
    int n;
    struct seg_node* root_segtree=node_new(NULL,NULL,0);
    // Reading Input from the User
    readInput(&n);
    //printf("yes\n");
    
    /* our tree is rooted at node 0 at depth 0 */
    int root = 0, parent_of_root=-1, depth_of_root=0;
 
    /* a DFS on the tree to set up:
     * arrays for parent, depth, subtree size for every node;
     * deeper end of every Edge */
    dfs(root, parent_of_root, depth_of_root, n);
    //printf("yes\n");

    int chain_heads[N];
 
    /*we have initialized no chain heads */
    memset(chain_heads, -1, sizeof(chain_heads));
 
    /* Stores number of edges for construction of segment
       tree. Initially we haven't traversed any Edges. */
    int edge_counted = 0;
 
    /* we start with filling the 0th chain */
    int curr_chain = 0;
 
    /* HLD of tree */
    hld(root, n-1, &edge_counted, &curr_chain, n, chain_heads);
    //printf("yes\n");
    /* ST of segregated Edges */
    //printf("%d\n",edge_counted);
    construct_ST(0,edge_counted,root_segtree);
    version[0]=root_segtree;
    //printf("yes\n");
    /*printf("%d %d %d\n",root_segtree->val, root_segtree->left->val, root_segtree->right->val);
    for(int i=0;i<edge_counted;i++){
        printf("%d ",base_array[i]);
    }*/
    /*Function to Input Queries and Process Them Accordingly*/
    processQueries(n,chain_heads);
    //printf("yes\n");
    //close(fp);
    return 0;
}
 
