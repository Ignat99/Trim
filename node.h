using namespace std;

typedef struct Point_struct { 
	int a1, a2, a3, a4, a5, a6, a7;
} point;
point Dop = { 24, 24, 24, 0, 0, 0, 0 };
point Faza = { 0, 0, 45, 0, 0, 0, 0 }; //Grad
point Sdvig = { 0, 0, 0, 0, 0, 0, 0 };
point Order = { 0, 1, 2, 3, 4, 5, 6 };
point c3 = { 0, 0, 1, 0, 0, 0, 0}; // coord virtual spase
point c4 = { 0, 0, 0, 0, 0, 0, 0}; // coord matrix array

//int xCursor, yCursor, t1, t2;
double indexCount = 0;

typedef union Point_union {
	point	P;
	int	Integer[7];
} point_u; 

point *c1 = new Point_struct;
point *c2 = new Point_struct;

typedef struct node_index {
	double index;
	int	str_len;
	char *name;
	double index_next;
	double index_prev;
	double index_top;
	double index_bottom;
	double index_sviaz_nos;
	double index_sviaz_hvost;
	point data;
} node_i;    



class Node 
{
protected:
public:
	Node (point*, char *, int);
	virtual ~Node (void);
	char *name;
	double index;
	int str_len;
	Node * _next;
	Node * _prev;
	Node * _top;
	Node * _bottom;
	Node * _sviaz_nos;
	Node * _sviaz_hvost;
	point data;
	Node *top(void);
	Node *bottom(void);
	Node *next(void);
	Node *prev(void);
	Node *insert(Node*);
	Node *remove(void);
	Node *get_name(int len) { name = new char[len]; }
};
Node::Node(point* cor, char *n, int len_str) :
_next(this), _prev(this)
{
//        _next = this;
//	_prev = 0;
//	_top = 0;
//	_sviaz_nos = 0;
//	_sviaz_hvost = 0;
	indexCount++; index = indexCount;
	data = *cor;
	str_len = len_str;
	name = new char[len_str];
	memset(name,0,len_str);
	// name = n;
	memcpy(name,n,len_str);
	_top = _bottom = _sviaz_nos = _sviaz_hvost = this;
	printf(" Node %d, %d, %d  name - %s  create \n", data.a1,data.a2,data.a3, n);
}
Node::~Node()
{
	printf(" Node %d, %d, %d  name - %s  destroid \n", data.a1,data.a2,data.a3, name);
	delete name;
}
Node *Node::next(void)
{
	return _next;
}
Node *Node::prev(void)
{
	return _prev;
}
Node *Node::top(void)
{
	return _top;
}
Node *Node::bottom(void)
{
	return _bottom;
}
Node *Node::insert(Node *b)
{
	Node *c = _next;
	b->_next = c;
	b->_prev = this;
	_next = b;
	c->_prev = b;
//	draw(this);
	return b;
}
Node *Node::remove(void)
{
	_prev->_next = _next;
	_next->_prev = _prev;
	_next = _prev = this;
	return this;
}
Node  * pCurNode, * pNewNode, * pSviazNos, * pSviazHvost, * pTopGrup, *pBottomGrup, * pVideo, * pFont, * pTmp, * pSetevaia;
struct node_data {
	char *name;
	Node * _next;
	Node * _prev;
	Node * _top;
	Node * _bottom;
	Node * _sviaz_nos;
	Node * _sviaz_hvost;
	point data;
} nd; 

Node * ReadIndex (void);
void WriteIndex (Node * , FILE * , FILE * );
void WriteIndexRec (Node * );

class compare {
public:
	bool operator() (const char *s, const char *t) const
	{	return strcmp(s, t) < 0;
	}
};

typedef void (*pOperator) ( Node *, Node *, Node *, int );
typedef multimap<char*, pOperator , compare> mmtype;
	mmtype MmapOperator;

void Vichislenie ( Node *a, Node *pCurOper, Node *pRezData, int i) // Proizvodimoe deistvie
{
    Node *pCurData, *pFirstRezData; //, *pCurOper, *pRezData;
    a = a->_bottom;
    pCurData = a;
//    pCurOper = a->_sviaz_nos;
//    pRezData = pCurOper->_sviaz_nos;

    if ( pRezData->_bottom ==  pRezData )
    {
	pRezData->_bottom = new Node(&pCurData->data,pCurData->name,pCurData->str_len);
	pRezData->_bottom->_top = pRezData;
	pRezData = pRezData->bottom();
	pFirstRezData = pRezData;
//	MovCurToNode(pCurNode);
    } else {
    pRezData = pRezData->bottom();
    pRezData = pRezData->insert(new Node(&pCurData->data,pCurData->name,pCurData->str_len));
    pRezData->_top = pRezData->_prev->_top;
    pFirstRezData = pRezData;
//    MovCurToNode(pCurNode);
    }
	do {
	    if ( a->_bottom->index != a->index )
		    Vichislenie(a->bottom(),pCurOper,pRezData,i);
	a = a->next();
	    pRezData = pRezData->insert(new Node(&a->data,a->name, a->str_len));
//	    pRezData->data.a1 = pRezData->data.a1 + 10*i;
	    pRezData->_top = pFirstRezData->_top;
	} while (a->next() != pCurData);
	if ( a->_bottom->index != a->index ) Vichislenie(a->bottom(),pCurOper,pRezData,i);
}
void HelloF ( Node *a, Node *pCurOper, Node *pRezData, int i) // Proizvodimoe deistvie
{
    fprintf(stdout, "Hello from MmapOperator. \n");
}

void OperatorCompil ( Node *a, Node *pCurOper, Node *pRezData, int i) // Proizvodimoe deistvie
{
    Node *pCurData, *pFirstRezData; //, *pCurOper, *pRezData;
    a = a->_bottom;
    pCurData = a;
 //    pCurOper = a->_sviaz_nos;
//    pRezData = pCurOper->_sviaz_nos;

    if ( pRezData->_bottom ==  pRezData )
    {
	pRezData->_bottom = new Node(&pCurData->data,pCurData->name,pCurData->str_len);
	pRezData->_bottom->_top = pRezData;
	pRezData = pRezData->bottom();
	pFirstRezData = pRezData;
//	MovCurToNode(pCurNode);
    } else {
    pRezData = pRezData->bottom();
	    pRezData->data.a1 = pRezData->data.a1 - pCurOper->_bottom->data.a1;
	    pRezData->data.a2 = pRezData->data.a2 - pCurOper->_bottom->data.a2;
	    pRezData->data.a3 = pRezData->data.a3 - pCurOper->_bottom->data.a3;
    pRezData = pRezData->insert(new Node(&pCurData->data,pCurData->name,pCurData->str_len));
    pRezData->_top = pRezData->_prev->_top;
    pFirstRezData = pRezData;
//    MovCurToNode(pCurNode);
    }
	    pRezData->data.a1 = pRezData->data.a1 - pCurOper->_bottom->data.a1;
	    pRezData->data.a2 = pRezData->data.a2 - pCurOper->_bottom->data.a2;
	    pRezData->data.a3 = pRezData->data.a3 - pCurOper->_bottom->data.a3;

//	a = a->next();
//    fprintf(stdout, "Name is: a = %s , pRezData = %s \n", a->name, pRezData->name);
	do {
	    if ( a->_bottom->index != a->index )
		    OperatorCompil(a->bottom(),pCurOper,pRezData,i);
	    a = a->next();
  pRezData = pRezData->insert(new Node(&a->data,a->name, a->str_len));
//	  fprintf(stdout, "Name is: a = %s , pRezData = %s \n", a->name, pRezData->name);

	    pRezData->data.a1 = pRezData->data.a1 - pCurOper->_bottom->data.a1;
	    pRezData->data.a2 = pRezData->data.a2 - pCurOper->_bottom->data.a2;
	    pRezData->data.a3 = pRezData->data.a3 - pCurOper->_bottom->data.a3;

	    pRezData->_top = pFirstRezData->_top;
	} while (a->next() != pCurData);

	    if ( a->_bottom->index != a->index ) OperatorCompil(a->bottom(),pCurOper,pRezData,i);
}

void VichisleniePot ( Node *a, Node *pCurOper, Node *pRezData, int i) // Proizvodimoe deistvie
{
    Node *pCurData, *pFirstRezData, *pCurParam, *pCurObj; //, *pCurOper, *pRezData;
    a = a->_bottom;
    pCurData = a;
    double Integer, rxr;
//    pCurOper = a->_sviaz_nos;
//    pRezData = pCurOper->_sviaz_nos;

    if ( pRezData->_bottom ==  pRezData )
    {
	pRezData->_bottom = new Node(&pCurData->data,pCurData->name,pCurData->str_len);
	pRezData->_bottom->_top = pRezData;
	pRezData = pRezData->bottom();
	pFirstRezData = pRezData;
    } else {
    pRezData = pRezData->bottom();
    pRezData = pRezData->insert(new Node(&pCurData->data,pCurData->name,pCurData->str_len));
    pRezData->_top = pRezData->_prev->_top;
    pFirstRezData = pRezData;
    }
//	    pRezData->data.a1 = pRezData->data.a1 - pCurOper->_bottom->data.a1;
	do {
	    if ( a->_bottom->index != a->index )
		    VichisleniePot(a->bottom(),pCurOper,pRezData,i);
	    a = a->next();
			if(!strcmp(pRezData->name, "M") ) pRezData->top()->data.a4 = pRezData->data.a4;
			if(!strcmp(pRezData->name, "VV_TWO") ) pRezData->top()->data.a5 = pRezData->data.a5;
	    pRezData = pRezData->insert(new Node(&a->data,a->name, a->str_len));
//	    pRezData->data.a1 = pRezData->data.a1 + 10*i;
	    pRezData->_top = pFirstRezData->_top;
			if(!strcmp(pRezData->name, "M") ) pRezData->top()->data.a4 = pRezData->data.a4;
			if(!strcmp(pRezData->name, "VV_TWO") ) pRezData->top()->data.a5 = pRezData->data.a5;
	} while (a->next() != pCurData);
	    if ( a->_bottom->index != a->index ) VichisleniePot(a->bottom(),pCurOper,pRezData,i);

	if (!strncmp(pRezData->name, "O_",2) )
	{
		pCurObj = pRezData;
	do {
    		fprintf(stdout, "Obj is: %s  \n", pCurObj->name);
		rxr = (double)(pCurObj->data.a1*pCurObj->data.a1
			+ pCurObj->data.a2*pCurObj->data.a2
			+ pCurObj->data.a3*pCurObj->data.a3);
		pCurObj->data.a6 = (int)(sqrt(rxr));
    		fprintf(stdout, "R x R is: %d  \n", pCurObj->data.a6);
		pCurParam = pCurObj->_bottom; a = pCurParam;
		do {
			if(!strcmp(pCurParam->name, "VV_TWO") && pCurParam->top()->data.a6 != 0 )
				pCurParam->data.a6 = (int)(pCurParam->top()->data.a4/pCurParam->top()->data.a6 + 0.5);
			pCurParam = pCurParam->next();
    		fprintf(stdout, "Param is: %s  \n", pCurParam->name);
		} while (pCurParam != a );
		pCurObj = pCurObj->next();

	} while ( pRezData != pCurObj);
	}
}

void OperatorF (Node *a) // Procedura zapuska ierarhii operatorov
{
    mmtype::iterator imm;
    Node * pData, * pOperator, * pZnach, * pVihod;
    int countNode = 0, i;
    pOperator = a;
    pData = a->_sviaz_hvost;
    pZnach = a->_bottom;
    pVihod = a->_sviaz_nos;
    a = pZnach;
//    do {
//	countNode++;
//	a = a->next();
//    } while ( a != pZnach );
//    for ( i = 0; i < countNode; i++ ) {
//	MmapOperator["OPERATOR_COPY"](pData,pOperator,pVihod,i);
	imm = MmapOperator.find(pOperator->name);
	if (imm != MmapOperator.end() ) (*imm).second (pData,pOperator,pVihod,i);
//    }
}
void WriteIndex (Node * pCurN, FILE * fp, FILE * fp1)
{
	Node * a; node_i ni;
	a = pCurN;
	do {
		ni.index = a->index;
		//		ni.name = a->name;
		//		a->name = strcat(a->name,"FFF");
		
		ni.index_next = a->_next->index;
		ni.index_prev = a->_prev->index;
		ni.index_top = a->_top->index;
		ni.index_bottom = a->_bottom->index;
		ni.index_sviaz_nos = a->_sviaz_nos->index;
		ni.index_sviaz_hvost = a->_sviaz_hvost->index;
		ni.data = a->data;
		ni.str_len = a->str_len;
		(void)fwrite((char *)&ni, sizeof(struct node_index), 1, fp);
		
		(void)fwrite(a->name, a->str_len, 1, fp);
		
		printf(" Write len %d  str %s   \n", ni.str_len, a->name);
		
		if ( (a->_bottom->index != a->index) && (a->_bottom->_top == a) ) 
			WriteIndex(a->_bottom, fp, fp1);
		a = a->next();
	} while (a != pCurN); 
}
void WriteIndexRec (Node * pCurN)
{
	Node * a;
	do {
		a = pCurN;
		pCurN = a->top();
	} while (a != pCurN); 
	FILE *fp, *fp1; 
	fp = fopen("data.cha", "w+");
	fp1 = fopen("text.cha", "w+");
	
	if(fp == NULL);
	WriteIndex (pCurN, fp, fp1);
	if(fclose(fp) == EOF);
	if(fclose(fp1) == EOF);
}
// Node * ReadIndex (Node * pCurN)
Node * ReadIndex (void)
{
	FILE *fp, *fp1; 
	Node * a, * b, * c, * d, *cDel; 
	node_i ni;
	int i = 0;
	char *str_name, *str_name1;
	fp = fopen("data.cha", "r");
	fp1 = fopen("text.cha", "r");
	
	if(fp == NULL) printf("Error read file \"data.cha\" ");
//	a = pCurN;
	a = new Node(c1,"A1\0",3);
	//a->name = new char[a->str_len];
	a->get_name(a->str_len);
	strcpy(a->name , "AA\0");
	
	d = new Node(c2,"D1\0",3);
	//d->name = new char[d->str_len];
	d->get_name(d->str_len);
	strcpy(d->name , "DA\0");
	
	indexCount--;
	
	while (  fread( (char *)&ni, sizeof(struct node_index), 1, fp) == 1  ) {
		printf("\n Index %f, Len %d , Next %f, Prev %f, top %f , bottom %f, Nos %f , Hvost %f \n", ni.index, ni.str_len, ni.index_next, ni.index_prev, ni.index_top, ni.index_bottom, ni.index_sviaz_nos, ni.index_sviaz_hvost);

		//if (  (fread ( (char *) &ni.str_len, sizeof(ni.str_len), 1, fp ) == 1) || (ferror(fp)==0) )  { printf("\nLen %d\n", ni.str_len); }
		str_name = new char[(int)ni.str_len];
		//str_name1 = new char[sizeof(struct node_index)];
		if(fread(str_name, ni.str_len , 1, fp) == 1) { printf("\nName %s\n", str_name ); }
		//if(fread(str_name1, sizeof(struct node_index) , 1, fp) == 1) { printf("\nSecond struct item %s\n", str_name ); }
		//if(fread( (char *)&ni.index_next, sizeof(ni.index_next), 1, fp) == 1) { printf("\nName %f\n", ni.index_next ); }
		//if(fread( (char *)&ni.index_prev, sizeof(ni.index_prev), 1, fp) == 1) { printf("\nName %f\n", ni.index_prev ); }
		//if(fread( (char *)&ni.index_top, sizeof(ni.index_top), 1, fp) == 1) { printf("\nName %f\n", ni.index_top ); }
		//if(fread( (char *)&ni.index_bottom, sizeof(ni.index_bottom), 1, fp) == 1) { printf("\nName %f\n", ni.index_bottom ); }
		//if(fread( (char *)&ni.index_sviaz_nos, sizeof(ni.index_sviaz_nos), 1, fp) == 1) { printf("\nName %f\n", ni.index_sviaz_nos ); }
		//if(fread( (char *)&ni.index_sviaz_hvost, sizeof(ni.index_sviaz_hvost), 1, fp) == 1) { printf("\nName %f\n", ni.index_sviaz_hvost ); }
		//if(fread( (char *)&ni.data, sizeof(ni.data), 1, fp) == 1) { printf("\nName %f\n", ni.data ); }


//		   printf(" Index %f , Str_len %d,  Name %c , Next %f, Prev %f, top %f , bottom %f, Nos %f , Hvost %f ", ni.index, ni.str_len, ni.name, ni.index_next, ni.index_prev, ni.index_top,
//			ni.index_bottom, ni.index_sviaz_nos, ni.index_sviaz_hvost);

//		printf("\nRead %d bytes  %s   \n", ni.str_len, str_name);
		
		b = new Node(c2, str_name, ni.str_len);
		delete str_name;
		
		b->index = ni.index;
		b->data = ni.data;
		
		d->_bottom = b;
		d = d->insert(new Node(c2,"LISTING\0",8));
		indexCount --;
		a = a->insert(b);
	} 
	cDel = d;
	d = d->prev();
	c = d;	// The End of Listing D
//		printf("Remove LISTING %d  %s   \n", i++, cDel->name);
		cDel = cDel->remove();
	delete cDel;
	
	(void)fseek(fp, (long)0, SEEK_SET);
	//	(void)fseek(fp1, (long)0, SEEK_SET);
	
/*
	while ( (fread((char *)&ni, sizeof(struct node_index), 1, fp) == 1) ) {
		
		//str_name = new char[(int)ni.str_len];
		//if(fread(str_name,(int) ni.str_len, 1, fp) == 1) printf("Second read %d  %s   \n", i++, str_name);
		printf("Second read %d  \n", i++);
		//delete str_name;
		
		while ( d->_bottom->index != ni.index) {
			d = d->next();
		}
		b = d->_bottom;
		while ( d->_bottom->index != ni.index_next ) {
			d = d->next();
		} 
		b->_next = d->_bottom;
		while ( d->_bottom->index != ni.index_prev ) {
			d = d->next();
		} 
		b->_prev = d->_bottom;
		while ( d->_bottom->index != ni.index_top ) {
			d = d->next();
		} 
		b->_top = d->_bottom;
		while ( d->_bottom->index != ni.index_bottom ) {
			d = d->next();
		} 
		b->_bottom = d->_bottom;
		while ( d->_bottom->index != ni.index_sviaz_nos ) {
			d = d->next();
		} 
		b->_sviaz_nos = d->_bottom;
		while ( d->_bottom->index != ni.index_sviaz_hvost ) {
			d = d->next();
		} 
		b->_sviaz_hvost = d->_bottom;
	}
*/
	d = c->next(); //The Bigin of Listing D
	do {
		printf("Remove %d %s    \n", --i, c->name);
		c = c->remove();
		c = d;
		d = d->next();
	} while (d != c); c = c->remove(); 
	if(fclose(fp) == EOF);
	if(fclose(fp1) == EOF);
	return a;
} 
