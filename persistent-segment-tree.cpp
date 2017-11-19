#include <bits/stdc++.h>
using namespace std;

#define MAXN 100


struct node
{
	
	int val;

	
	node* left, *right;

	node() {}
	node(node* l, node* r, int v)
	{
		left = l;
		right = r;
		val = v;
	}
};


int arr[MAXN];


node* version[MAXN];


void build(node* n,int low,int high)
{
	if (low==high)
	{
		n->val = arr[low];
		return;
	}
	int mid = (low+high) / 2;
	n->left = new node(NULL, NULL, 0);
	n->right = new node(NULL, NULL, 0);
	build(n->left, low, mid);
	build(n->right, mid+1, high);
	n->val = n->left->val + n->right->val;
}

void upgrade(node* prev, node* cur, int low, int high,int idx, int value)
{
	if (idx > high or idx < low or low > high)
		return;

	if (low == high)
	{
		
		cur->val = value;
		return;
	}
	int mid = (low+high) / 2;
	if (idx <= mid)
	{
		
				cur->right = prev->right;

		
		cur->left = new node(NULL, NULL, 0);

		upgrade(prev->left,cur->left, low, mid, idx, value);
	}
	else
	{
		
		cur->left = prev->left;

		
		cur->right = new node(NULL, NULL, 0);

		upgrade(prev->right, cur->right, mid+1, high, idx, value);
	}

	
	cur->val = cur->left->val + cur->right->val;
}

int query(node* n, int low, int high, int l, int r)
{
	if (l > high or r < low or low > high)
	return 0;
	if (l <= low and high <= r)
	return n->val;
	int mid = (low+high) / 2;
	int p1 = query(n->left,low,mid,l,r);
	int p2 = query(n->right,mid+1,high,l,r);
	return p1+p2;
}

int main(int argc, char const *argv[])
{
	int n;
	
cout<<"Enter no.of elements:";
cin>>n;
int a[n];
cout<<"Enter elements";
	for (int i=0; i<n; i++) 
	cin>>a[i];
	for(int i=0;i<n;i++ )
	arr[i]=a[i];

	
	node* root = new node(NULL, NULL, 0);
	build(root, 0, n-1);

	
	version[0] = root;


	cout<<"Enter the no.of versions/update of the tree:";
	int v,k=1;
	cin>>v;
	while(v--)
	{
	cout<<"Enter the update index and value:";
	int ui,uv;
	cin>>ui>>uv;
	version[k] = new node(NULL, NULL, 0);
	upgrade(version[k-1], version[k], 0, n-1, ui, uv);
	cout<<"Version "<<k<<" is built\n";
	k++;
	
}
	
cout<<"Enter no. of queries:";
int q;
cin>>q;
while(q--)
{
	int vno;
	cout<<"Enter the version no. to query:";
	cin>>vno;
	cout<<"Enter the range:";
	int ql,qr;
	cin>>ql>>qr;
	cout << "In version "<<vno<<" the sum from "<<ql<<" to "<<qr<<" is ";
	cout << query(version[vno], 0, n-1, ql, qr) << endl;


}
	return 0;
}
