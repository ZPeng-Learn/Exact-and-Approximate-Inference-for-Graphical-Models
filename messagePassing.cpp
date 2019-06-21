#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <map>
#include <math.h>
#include <queue>

using namespace std;

int v[100];
int adjacency[100][100], tempAdjacency[100][100];
int n1, n2, totalVar;
int ordering[100];
int vEliminated[100];
vector<vector<double> > ocrFactor(1000,vector<double>(10)); // 1 vector of size 10 for each image id
vector<vector<double> > transFactor(10,vector<double>(10)); // 10*10 table for each occurrence possibility
map<char,int> mapping;

vector<double> tempMsg;
vector<int> tempScope;

int modelNum;


int totalWord= 0;;

class node{

public:
	

	vector<double> factor;

	int factorSize;

	vector<int> scope;

	vector<vector<int> > factorIndex;

};

void up_MSG(int index, vector<int>& child, map<int, class node * > &cliques, vector<int> &tempScope, vector<vector<int> > &scopeOfUpMsg, vector<double> &tempMsg, vector<vector<double> > &upMsg){

	class node *tempNode = cliques[index];
	
	int scopeSize = tempNode->scope.size();
	// adding scope
	

	for(int i=1;i<scopeSize;i++){

		tempScope.push_back(tempNode->scope[i]);
	}

	vector<double > tempFactor;

	tempFactor = tempNode->factor;

	int ll = tempFactor.size();
	
	map<string,double> mapping1;
	
	int childSize = child.size();

	int fSize = tempNode->factorSize; 

	int newTableSize = tempNode->factorSize;
	
	newTableSize /=10;

	for(int i=0;i<childSize;i++){
		
		// pick one child at a time
		int l1 = scopeOfUpMsg[child[i]].size(); 
		vector<int> in(l1);
		for(int k=0;k<l1;k++)
			in[k] = 0;
		
		for(int k=0;k<newTableSize;k++){

			string s = "";
			for(int z=0;z<l1;z++)
				s += to_string(in[z]);

			mapping1[s] = upMsg[child[i]][k];

			in[l1-1]++;

			for(int l=l1-1;l>0;l--){

				if(in[l]==10){
					in[l-1]++;
					in[l] = 0;
				}
			}

		}

		vector<int> order;

		for(int l=0;l<l1;l++){

			for(int z=0;z<scopeSize;z++){
				
				if(scopeOfUpMsg[child[i]][l] == tempNode->scope[z]){
					order.push_back(z);
					break;
				}
			}
		}

		for(int l=0;l<fSize;l++){

			string tempString = "";
			for(int z=0;z<l1;z++){
				
				tempString += to_string(tempNode->factorIndex[l][order[z]]);
			}

			tempFactor[l] += mapping1[tempString];
			
		}

	}

	
	double tempValue;
	for(int i=0;i<newTableSize;i++){

		tempValue = 0.0;
		
		for(int j=i;j<tempNode->factorSize;j = j + newTableSize){
			
			tempValue += exp(tempFactor[j]);
			
		}
		tempValue = log(tempValue);
		tempMsg.push_back(tempValue);

	}
}


void createBelief(int index, map<int, class node*> &cliques, vector<vector<int> > &scopeOfUpMsg, vector<int> &upNbr, vector<vector<double> > &downMsg, vector<double> &tempMsg, vector<vector<int> > &downNbr, vector<vector<double> > &upMsg, vector<vector<double> > &beliefOfCliqueNodes){

	class node *tempNode = cliques[index];
	tempMsg = tempNode->factor;

	int tempSize = scopeOfUpMsg[index].size();

	map<string,double> mapping1;

	int newTableSize = pow(10,tempSize);

	int scopeSize = tempNode->scope.size();

	int fSize = pow(10,scopeSize);

	vector<double> tempFactor;


	if(upNbr[index]!=-1){
	
	tempFactor  = downMsg[index];
	
	vector<int> in(tempSize);

	for(int k=0;k<tempSize;k++)
		in[k] = 0;

	for(int k=0;k<newTableSize;k++){

		string s = "";
		for(int z=0;z<tempSize;z++)
			s += to_string(in[z]);
		
		mapping1[s] = tempFactor[k];

		in[tempSize-1]++;

		for(int l=tempSize-1;l>0;l--){

			if(in[l]==10){
				in[l-1]++;
				in[l] = 0;
			}
		}
	}
	
	for(int i=0;i<newTableSize && index == 7;i++)
		cout<<tempFactor[i]<<"\n";
	
	vector<int> order;
	for(int i=0;i<tempSize;i++){
		for(int j=0;j<scopeSize;j++){

			if(scopeOfUpMsg[index][i] == tempNode->scope[j]){
				order.push_back(j);
				break;

			}
		}

	}
	for(int l=0;l<fSize;l++){

			string tempString = "";
			for(int z=0;z<tempSize;z++){
				
				tempString += to_string(tempNode->factorIndex[l][order[z]]);
			}
		
			tempMsg[l] += mapping1[tempString];

	}


	mapping1.clear();
	}
	// take msg from all down neighbour and update belief

	int downNbrSize = downNbr[index].size();

	for(int i=0;i<downNbrSize;i++){

		tempSize = scopeOfUpMsg[downNbr[index][i]].size();

		tempFactor = upMsg[downNbr[index][i]];

		newTableSize = pow(10,tempSize);

		vector<int> in(tempSize);

		for(int k=0;k<tempSize;k++)
			in[k] = 0;

		for(int k=0;k<newTableSize;k++){

			string s = "";
			for(int z=0;z<tempSize;z++)
				s += to_string(in[z]);

			mapping1[s] = tempFactor[k];

			in[tempSize-1]++;

			for(int l=tempSize-1;l>0;l--){

				if(in[l]==10){
					in[l-1]++;
					in[l] = 0;
				}


			}

		}

		vector<int> order;
		for(int k=0;k<tempSize;k++){
			for(int j=0;j<scopeSize;j++){

				if(scopeOfUpMsg[downNbr[index][i]][k] == tempNode->scope[j]){
					order.push_back(j);
					break;

				}
			}

		}

		for(int l=0;l<fSize;l++){

				string tempString = "";
				for(int z=0;z<tempSize;z++){
					
					tempString += to_string(tempNode->factorIndex[l][order[z]]);
				}
				tempMsg[l] += mapping1[tempString];

		}

		mapping1.clear();
	}
	beliefOfCliqueNodes[index] = tempMsg;

}



int calEdgeAdded(int index){

	vector<int> a;
	

	for(int i=0;i<totalVar;i++){

		if(tempAdjacency[index][i]==1 && v[i]==0)
			a.push_back(i);
	}

	int l = a.size();
	int count = 0;
	for(int i=0;i<l-1;i++){
		for(int j=i+1;j<l;j++){

			if(tempAdjacency[a[i]][a[j]]==0)
				count++;
		}
	}

	return count;

}


int main(){


	ifstream file;
	ifstream fileOCR;
	ifstream fileTrans;
	ifstream fileTruth;
	string temp;
	int a1[100],a2[100];

	char temp1[1000];

	char *c;

	cin>>modelNum;

	// modelNum = 1 OCR, 2 = OCR + trans, 3 = OCR+trans+skip, 4 = complete

	file.open("../OCRdataset-2/data/data-loops.dat");
	fileOCR.open("../OCRdataset-2/potentials/ocr.dat");
	fileTrans.open("../OCRdataset-2/potentials/trans.dat");
	fileTruth.open("../OCRdataset-2/data/truth-loops.dat");

	int totalChar = 0;

	// mapping of character to index
	mapping['d'] = 0;
	mapping['o'] = 1;
	mapping['i'] = 2;
	mapping['r'] = 3;
	mapping['a'] = 4;
	mapping['h'] = 5;
	mapping['t'] = 6;
	mapping['n'] = 7;
	mapping['s'] = 8;
	mapping['e'] = 9;

	int r=0, in=0;

	while(!fileOCR.eof()){
		getline(fileOCR,temp);
		strcpy(temp1, temp.c_str());

		

		c = strtok(temp1,"\t");
		while(c!=NULL){

			double temp = atof(c);
			c  = strtok(NULL,"\t");
			if(c==NULL){
				
				ocrFactor[r][in] = log(temp);

				in++;
				if(in==10){
					r++;
					in = 0;
				}

			}
		}
	}

	r = 0;
	in = 0;
	int index1, index2;
	while(!fileTrans.eof()){

		getline(fileTrans,temp);
		strcpy(temp1, temp.c_str());
		
		

		c = strtok(temp1,"\t");
		while(c!=NULL){
			

			index1 = mapping[c[0]];
			c  = strtok(NULL,"\t");
			index2 = mapping[c[0]];
			//cout<<c[0]<<"\n";
			c  = strtok(NULL,"\t");
			transFactor[index1][index2] = log(atof(c));
			

			c  = strtok(NULL,"\t");
			
		}

		
		
	}
	
	
	int ccounter = 0;

	int characterCounter = 0;

	int correctWord = 0;

	double DLL = 0.0;

	while(!file.eof()){

		 n1  = 0, n2  = 0;

		ccounter++;
		//cout<<ccounter<<"hello\n";


		map<int, class node *> cliques;
		vector<vector<int> > scopeOfNewFactor;
		vector<int> visitedNewFactor;
		vector<int> sourceNewFactor;
		int cliqueAdjacency[100][100];

		int transAdded[100];
		

		vector<vector<double> > upMsg(20);
		vector<vector<double> > downMsg(20);
		vector<vector<int> > scopeOfUpMsg(20);
		vector<vector<double> > beliefOfCliqueNodes(20);
		vector<vector<int> > maxUpMsg(20);

		vector<int> upNbr(20,-1);
		vector<vector<int> > downNbr(20);


		int a3[100],a4[100];
		//getline(fileTruth,temp);getline(fileTruth,temp);getline(fileTruth,temp);
		getline(fileTruth,temp);
		int len = temp.length();

		for(int i=0;i<len;i++){

			a3[i] = mapping[temp[i]];
			
		}

		getline(fileTruth,temp);
		len = temp.length();

		for(int i=0;i<len;i++){

			a4[i] = mapping[temp[i]];
			
		}

		
		getline(file,temp);
		
		if(temp[0]=='\0')
			break;
		strcpy(temp1, temp.c_str());
		c = strtok(temp1,"\t");
		while(c!=NULL){

			a1[n1++] = atoi(c);
			
			c  = strtok(NULL,"\t");
			

		}
		
		for(int i=0;i<totalVar;i++)
			transAdded[i] = 0;

		getline(file,temp);
		strcpy(temp1, temp.c_str());

		c = strtok(temp1,"\t");
		while(c!=NULL){

			a2[n2++] = atoi(c);
			
			c  = strtok(NULL,"\t");
			

		}

		getline(file,temp);
		getline(fileTruth,temp);
		
		totalVar = n1 + n2;
		for(int i=0;i<n1+n2;i++){

			for(int j=0;j<n1+n2;j++){

				adjacency[i][j] = 0;
			}
		}

		if(modelNum!=1){
			// adding trans edge

			for(int i=0;i<n1-1;i++){

				adjacency[i][i+1] = 1;
				adjacency[i+1][i] = 1;

			}

			for(int i=0;i<n2-1;i++){

				adjacency[n1+i][n1+i+1] = 1;
				adjacency[n1+i+1][n1+i] = 1;
				
			}
		}
		

		if(modelNum!=1 && modelNum!=2){
		// skip factor edge

			for(int i=0;i<n1-1;i++){
				for(int j=i+1;j<n1;j++){

					if(a1[i]==a1[j]){
						adjacency[i][j] = 1;
						adjacency[j][i] = 1;

					}
				}
			}

			for(int i=0;i<n2-1;i++){
				for(int j=i+1;j<n2;j++){

					if(a2[i]==a2[j]){
						adjacency[n1+i][n1+j] = 1;
						adjacency[n1+j][n1+i] = 1;
						
					}
				}
			}

		}

		if(modelNum == 4){
		// word - skip factor

			for(int i=0;i<n1;i++){

				for(int j=0;j<n2;j++){

					if(a1[i]==a2[j]){

						adjacency[i][n1+j] = 1;
						adjacency[n1+j][i] = 1;

					}
				}
			}
		}
		//*
		/*
		cout<<"Adjacency Matrix : \n";

		for(int i=0;i<totalVar;i++){
			for(int j=0;j<totalVar;j++){

				cout<<adjacency[i][j]<<" ";
			}
			cout<<"\n";
		}

		cout<<"END\n\n";
		//*/
		
		for(int i=0;i<totalVar;i++)
			for(int j=0;j<totalVar;j++)
				tempAdjacency[i][j] = adjacency[i][j];

		// finding elimination ordering using min-fill algorithm
		
		vector<int> a;

		for(int i=0;i<totalVar;i++)
			v[i] = 0;

		int countOfElimination = 0;

		while(countOfElimination != totalVar){
			int temp = INT_MAX, index;
			int t;
			for(int i=0;i<totalVar;i++){
				if(v[i]==0){
					t = calEdgeAdded(i);
					//cout<<t<<" ";
					if(t<temp){
						temp = t;
						index = i;
					}
				}


			}
			//cout<<"\n";
			a.clear();

			for(int i=0;i<totalVar;i++){

				if(tempAdjacency[index][i]==1 && v[i]==0)
					a.push_back(i);
			}

			int l = a.size();

			for(int i=0;i<l-1;i++){

				for(int j=0;j<l;j++){

					if(tempAdjacency[a[i]][a[j]]==0){
						tempAdjacency[a[i]][a[j]] = 1;
					}
				}
			}
			ordering[countOfElimination] = index;
			v[index] = 1;
			countOfElimination++;
		}
		
		
						
		for(int j=0;j<totalVar;j++){
			for(int k=0;k<totalVar;k++)
				cliqueAdjacency[j][k]=0;
		}		
		// Insert OCR factor Node 10*1 because Image Id is fixed
		
		// clique numbering from 0 

		// create clique tree
		for(int i=0;i<totalVar;i++)
			vEliminated[i] = 0;


		vector<int> p;

		int pSize;
		int fSize;
		int newFSize;
		


		for(int i=0;i<totalVar;i++){
			p.clear();
				
			class node *temp3 = new node;
			
			
			vEliminated[ordering[i]] = 1;
			//cout<<"Ordering"<<ordering[i]<<"\n";
			for(int j=0;j<totalVar;j++){

				if(adjacency[ordering[i]][j] == 1 && vEliminated[j]==0)
					p.push_back(j);

			}
			/*
			for(int l=0;l<p.size();l++){
				cout<<p[l]<<" ";
			}
			cout<<"\n";
			*/
			// check for factors which are created newly
			newFSize = scopeOfNewFactor.size();
			//cout<<newFSize<<" "<<i<<"\n";
			for(int k=0;k<newFSize;k++){

				int ll = scopeOfNewFactor[k].size();
				/*for(int ll1 = 0;ll1<ll;ll1++)
					cout<<scopeOfNewFactor[k][ll1]<<" ";
				cout<<"\n";
				*/
				if(visitedNewFactor[k]==0){

					if(find(scopeOfNewFactor[k].begin(),scopeOfNewFactor[k].end(),ordering[i])!=scopeOfNewFactor[k].end()){
						//cout<<"here\n";
						int m = scopeOfNewFactor[k].size();
						for(int l=0;l<m;l++){

							if(find(p.begin(),p.end(),scopeOfNewFactor[k][l])==p.end() && scopeOfNewFactor[k][l]!=ordering[i])
								p.push_back(scopeOfNewFactor[k][l]);
						}

						visitedNewFactor[k] = 1;	// removing that factor
						//cout<<"ADD Edge :\n";
						//cout<<i<<" "<<sourceNewFactor[k]<<" "<<k<<"\n";
						cliqueAdjacency[i][sourceNewFactor[k]] = 1;	// adding edges in clique tree
						cliqueAdjacency[sourceNewFactor[k]][i] = 1;
					}
				}
			}


			pSize = p.size();


			if(pSize != 0){

				// a new msg created
				sourceNewFactor.push_back(i);
				scopeOfNewFactor.push_back(p);
				visitedNewFactor.push_back(0);

			}
			
			
			temp3->scope.push_back(ordering[i]);

			for(int j=0;j<pSize;j++)
				temp3->scope.push_back(p[j]);


			pSize++;
			p.clear();
			for(int j=0;j<pSize;j++){
				p.push_back(temp3->scope[j]);
			}

			fSize = pow(10,pSize);

			temp3->factorSize = fSize;
			// Initializing ocrFactor

			int q=0;
			int imageId;

			if(ordering[i]>=n1){

				imageId = a2[ordering[i]-n1];

			}else{

				imageId = a1[ordering[i]];


			}

			vector<int> in(pSize);
			for(int k=0;k<pSize;k++)
				in[k] = 0;

			for(int k=0;k<fSize;k++){

				temp3->factorIndex.push_back(in);
				in[pSize-1]++;

				for(int l=pSize-1;l>0;l--){

					if(in[l]==10){
						in[l-1]++;
						in[l] = 0;
					}
				}

			}
			//cout<<fSize<<"\n";
			// updating OCR potentials, it's always on first column of index
			for(int k=0;k<fSize;k++){
				//cout<<ocrFactor[imageId][temp3->factorIndex[k][0]]<<"\n";
				temp3->factor.push_back(ocrFactor[imageId][temp3->factorIndex[k][0]]);
				//cout<<imageId<<" "<<temp3->factorIndex[k][0]<<" "<<exp(ocrFactor[imageId][temp3->factorIndex[k][0]])<<"\n";

			}

			// updating transFactor

			if(modelNum!=1){
			
				int tempTrans = -1;
				if(ordering[i]!=n1-1 && ordering[i]!=totalVar-1 && vEliminated[ordering[i]+1]==0){
					for(int k=0;k<pSize;k++){

						if(p[k]==ordering[i]+1){

							tempTrans = k;
							break;

						}

					}
					
					if(tempTrans != -1){
						for(int k=0;k<fSize;k++){
							temp3->factor[k] += transFactor[temp3->factorIndex[k][0]][temp3->factorIndex[k][tempTrans]];
							//cout<<temp3->factor[k]<<" "<<temp3->factorIndex[k][0]<<" "<<temp3->factorIndex[k][tempTrans]<<"\n";

						}
					}
				}
				
				
				tempTrans = -1;
				if(ordering[i]!=0 && ordering[i]!=n1 && vEliminated[ordering[i]-1]==0){
					
					for(int k=0;k<pSize;k++){

						if(p[k]==ordering[i]-1){

							tempTrans = k;
							break;

						}

					}
					//cout<<tempTrans<<"\n";
					if(tempTrans != -1){
						for(int k=0;k<fSize;k++){
							temp3->factor[k] += transFactor[temp3->factorIndex[k][tempTrans]][temp3->factorIndex[k][0]];
							//cout<<temp3->factor[k]<<" "<<temp3->factorIndex[k][tempTrans]<<" "<<temp3->factorIndex[k][0]<<" "<<transFactor[temp3->factorIndex[k][tempTrans]][temp3->factorIndex[k][0]]<<"\n";

						}
					}
				}
			}

			
			// updating skip factor and word-skip

			if(ordering[i]<n1){

				// updating skip
				int tempImg1 = a1[ordering[i]];

				if(modelNum!=1 && modelNum!=2){
					for(int k=0;k<n1;k++){
						int tempSkip1;
						if(k!=ordering[i] && tempImg1 == a1[k] && vEliminated[k]==0){
							// same Image Id, adding skip factors
							
							for(int l=0;l<pSize;l++){
								if(p[l]==k){
									tempSkip1 = l;
									break;
								}

							}

							// add skip factor 

							for(int l=0;l<fSize;l++){

								if(temp3->factorIndex[l][0]==temp3->factorIndex[l][tempSkip1]){
									temp3->factor[l] += log(5);
								}else{
									temp3->factor[l] +=log(1);

								}
							}

						}


					}
				}

				if(modelNum==4){
				//updating word skip
					for(int k=n1;k<totalVar;k++){
						int tempSkip1;
						if( tempImg1 == a2[k-n1] && vEliminated[k]==0){


							for(int l=0;l<pSize;l++){
								if(p[l]==k){
									tempSkip1 = l;
									break;
								}

							}

							// add word-skip factor

							for(int l=0;l<fSize;l++){

								if(temp3->factorIndex[l][0] == temp3->factorIndex[l][tempSkip1]){

									temp3->factor[l] += log(5);

								}else{

									temp3->factor[l] += log(1);

								}

							}


						}

					}
				}


			}else{

				// updating skip

				int tempImg1 = a2[ordering[i] - n1];

				if(modelNum!=1 && modelNum!=2){

					for(int k=n1;k<totalVar;k++){
						int tempSkip1;
						if(k!=ordering[i] && tempImg1 == a2[k-n1] && vEliminated[k]==0){
							// same Image Id, adding skip factors
							
							for(int l=0;l<pSize;l++){
								if(p[l]==k){
									tempSkip1 = l;
									break;
								}

							}

							// add skip factor 

							for(int l=0;l<fSize;l++){

								if(temp3->factorIndex[l][0]==temp3->factorIndex[l][tempSkip1]){
									temp3->factor[l] += log(5);
								}else{
									temp3->factor[l] +=log(1);

								}
							}

						}


					}
				}

				if(modelNum==4){

				//updating word skip
					for(int k=0;k<n1;k++){
						int tempSkip1;
						if( tempImg1 == a1[k] && vEliminated[k]==0){


							for(int l=0;l<pSize;l++){
								if(p[l]==k){
									tempSkip1 = l;
									break;
								}

							}

							// add word-skip factor

							for(int l=0;l<fSize;l++){

								if(temp3->factorIndex[l][0] == temp3->factorIndex[l][tempSkip1]){

									temp3->factor[l] += log(5);

								}else{

									temp3->factor[l] += log(1);

								}

							}


						}


					}
				}
			}
			
			cliques[i] = temp3;
			
		}

		if(modelNum == 1){

			int assignment[totalVar];

			for(int i=0;i<totalVar;i++){

				class node *tempNode = cliques[i];

				int fSize = tempNode->factorSize;

				double maxValue = exp(tempNode->factor[0]);
				int maxIndex = 0;
				double z = maxValue;
				for(int j=1;j<fSize;j++){
					double potentialMax = exp(tempNode->factor[j]);

					z += potentialMax;
					if(maxValue < potentialMax){

						maxValue = potentialMax;
						maxIndex = j;
						
					}
					
				}
				double num = exp(tempNode->factor[a3[i]]);
				num = num/z;

				DLL += log(num);
				assignment[ordering[i]] = maxIndex;

			}

			int prevcharCount = characterCounter;
			for(int i=0;i<n1;i++){

				if(assignment[i] == a3[i])
					characterCounter++;
				totalChar++;
			}

			if((characterCounter - prevcharCount) == n1)
				correctWord++;

			prevcharCount = characterCounter;

			for(int i=0;i<n2;i++){

				if(assignment[n1+i] == a4[i])
					characterCounter++;
				totalChar++;
			}

			if((characterCounter - prevcharCount) == n2)
				correctWord++;

		}else if(modelNum == 2 || modelNum == 3){
		int root = n1 - 1;
		
		totalVar = n1;

		for(int j=0;j<totalVar;j++)
			upNbr.push_back(-1);

		int vEle[totalVar];
		for(int j=0;j<totalVar;j++)
			vEle[j] =0;

		queue<int> q;

		q.push(root);

		vEle[totalVar-1] = 1;
	
		while(!q.empty()){

			int tempRoot = q.front();
			q.pop();
			//cout<<tempRoot<<endl;

			for(int j=0;j<totalVar;j++){

				if(j!=tempRoot && vEle[j]==0 && cliqueAdjacency[tempRoot][j]==1){
					//cout<<j<<endl;
					upNbr[j] = tempRoot;
					vEle[j] = 1;
					q.push(j);
				}
			}


		}
		root = totalVar-1;


		q.push(root);
		
		for(int i=0;i<totalVar;i++)
			vEle[i] = 0;
		vEle[root] = 1;
		while(!q.empty()){

			int tempRoot = q.front();
			q.pop();

			for(int j=0;j<totalVar;j++){

				if(j!=tempRoot && vEle[j]==0 && cliqueAdjacency[tempRoot][j]==1){
						downNbr[tempRoot].push_back(j);
						vEle[j] = 1;
						q.push(j);
				}
			}

		}
		// two-way message passing
		// upward pass
		vector<int> countUpMsg(totalVar,0);

		int downNbrRoot = downNbr[totalVar-1].size();
		int vUp[totalVar];
		for(int i=0;i<totalVar-1;i++)
			vUp[i] = 0;

		while(downNbrRoot != countUpMsg[totalVar-1]){

			for(int i=0;i<totalVar-1;i++){
				int tempSize = downNbr[i].size();
				
				if(tempSize == countUpMsg[i] && vUp[i]==0){
					vector<double> tempMsg;
					vector<int> tempScope;
					vUp[i] = 1;
					int j = upNbr[i];
					countUpMsg[j]++;
			
					up_MSG(i, downNbr[i], cliques, tempScope, scopeOfUpMsg, tempMsg, upMsg);
					//cout<<"here3\n";
					upMsg[i] = tempMsg;
					scopeOfUpMsg[i] = tempScope;
					
				}
			}


		}

		int upMsgPresent[totalVar][totalVar];
		int downMsgPresent[totalVar][totalVar];
		int countDownMsg = 0;

		for(int i=0;i<totalVar;i++){

			for(int j=0;j<totalVar;j++){
				
				upMsgPresent[i][j] = 0;
				downMsgPresent[j][i] = 0;

				if(upNbr[i]==j){

					upMsgPresent[i][j] = 1;
					downMsgPresent[j][i] = 1;
					countDownMsg++;

				}


			}
		}
		
		
		int vDown[totalVar];
		for(int i=0;i<totalVar;i++)
			vDown[i] = 0;
		vector<double> downFactor;
		vector<double> beliefFactor;
		map<string,double> mapping;
		countDownMsg = totalVar;

		while(countDownMsg!=0){
			
			for(int i=0;i<totalVar;i++){

				if((upNbr[i]==-1 || vDown[upNbr[i]] == 1 )&& vDown[i]==0){
					//cout<<"here\n";
					
					countDownMsg--;
					vDown[i] = 1;
					
					class node *tempNode = cliques[i];

					int tempDownNbrSize = downNbr[i].size();

					
					tempMsg.clear();
					tempScope.clear();


					createBelief(i, cliques, scopeOfUpMsg, upNbr, downMsg, tempMsg, downNbr, upMsg, beliefOfCliqueNodes);
					for(int j=0;j<tempDownNbrSize;j++){

						// send msg to down nbr j
						// divide by factor coming from downNr[i][j]
						beliefFactor = beliefOfCliqueNodes[i];


						downFactor = upMsg[downNbr[i][j]];
						int tempSize = scopeOfUpMsg[downNbr[i][j]].size();
						int newTableSize = pow(10,tempSize);
						int scopeSize = tempNode->scope.size();
						int fSize = pow(10,scopeSize);

						vector<int> in(tempSize);

						for(int k=0;k<tempSize;k++)
							in[k] = 0;

						for(int k=0;k<newTableSize;k++){

							string s = "";
							for(int z=0;z<tempSize;z++)
								s += to_string(in[z]);

							mapping[s] = downFactor[k];

							in[tempSize-1]++;

							for(int l=tempSize-1;l>0;l--){

								if(in[l]==10){
									in[l-1]++;
									in[l] = 0;
								}
							}
						}

						vector<int> order;

						for(int k=0;k<tempSize;k++){
							for(int j1=0;j1<scopeSize;j1++){

								if(scopeOfUpMsg[downNbr[i][j]][k] == tempNode->scope[j1]){
									order.push_back(j1);
									break;

								}
							}

						}

						for(int l=0;l<fSize;l++){

								string tempString = "";
								for(int z=0;z<tempSize;z++){
									
									tempString += to_string(tempNode->factorIndex[l][order[z]]);
								}


								beliefFactor[l] -= mapping[tempString];
								
						}
						
						mapping.clear();

						for(int k=0;k<tempSize;k++)
							in[k] = 0;

						for(int k=0;k<newTableSize;k++){

							string s = "";
							for(int z=0;z<tempSize;z++)
								s += to_string(in[z]);

							mapping[s] = 0.0;
							

							in[tempSize-1]++;

							for(int l=tempSize-1;l>0;l--){

								if(in[l]==10){
									in[l-1]++;
									in[l] = 0;
								}
							}

						}

						for(int l=0;l<fSize;l++){

								string tempString = "";
								for(int z=0;z<tempSize;z++){
									
									tempString += to_string(tempNode->factorIndex[l][order[z]]);
								}
								mapping[tempString] += exp(beliefFactor[l]);


						}

						for(int k=0;k<tempSize;k++)
							in[k] = 0;


						for(int k=0;k<newTableSize;k++){

							string s = "";
							for(int z=0;z<tempSize;z++)
								s += to_string(in[z]);

							downFactor[k] = log(mapping[s]);
							

							in[tempSize-1]++;

							for(int l=tempSize-1;l>0;l--){

								if(in[l]==10){
									in[l-1]++;
									in[l] = 0;
								}
							}

						}

						// msg created

						downMsg[downNbr[i][j]] = downFactor;

						downFactor.clear();
					}

				
				}
		
			}
		}
		int assignment[10];


		for(int i=0;i<totalVar-1;i++){

			double marginalBelief[10];

			for(int j=0;j<10;j++)
				marginalBelief[j] = 0.0;

			int count= 0;

			int remainingTableSize = upMsg[i].size();

			int fSize = cliques[i]->factorSize;


			vector<double> factor = beliefOfCliqueNodes[i];

			int startIndex  = 0;
			double z= 0.0;
			for(int j=0;j<10;j++){


				int endIndex = startIndex + remainingTableSize;

				for(int k= startIndex;k<endIndex;k++){

					marginalBelief[j] += exp(factor[k]);
				}
				marginalBelief[j] = log(marginalBelief[j]);

				startIndex = startIndex + remainingTableSize;
			}

			double maxValue = exp(marginalBelief[0]);
			int ass = 0;
			z = maxValue;

			for(int i1=1;i1<10;i1++){

				double potentialMax = exp(marginalBelief[i1]);
				z += potentialMax;

				if(potentialMax > maxValue){

					maxValue = potentialMax;
					ass = i1;
				}
			}

			assignment[ordering[i]] = ass;

			if(ordering[i] < n1){

			double num = exp(marginalBelief[a3[ordering[i]]]);

			num = num / z;

			//cout<<num<<" "<<z<<"\n";

			DLL += log(num);
		}
			//cout<<DLL<<"\n";

		}

		

		vector<double> factor = beliefOfCliqueNodes[totalVar-1];
		double maxValue = exp(factor[0]);
		int ass = 0;

		double z = maxValue;
		for(int i=1;i<10;i++){

			double potentialMax = exp(factor[i]);
			z += potentialMax;
			if(potentialMax > maxValue){

				maxValue = potentialMax;
				ass = i;
			}

		}
		if(ordering[totalVar-1] < n1){

		double num = exp(factor[a3[ordering[totalVar-1]]]);
		
		num = num/z;

		DLL += log(num);
		}
		

		assignment[ordering[totalVar-1]] = ass;

		int prevcharCount = characterCounter;
		for(int i=0;i<n1;i++){

			if(assignment[i] == a3[i])
				characterCounter++;
			totalChar++;
		}

		if((characterCounter - prevcharCount) == n1)
				correctWord++;
		// do for second word
		// change adjancency list

		if(n2 > 0){
		n1  = 0;
		totalVar = n1 + n2;


		map<int, class node *> cliques;
		vector<vector<int> > scopeOfNewFactor;
		vector<int> visitedNewFactor;
		vector<int> sourceNewFactor;
		int cliqueAdjacency[100][100];

		int transAdded[100];
		

		vector<vector<double> > upMsg(20);
		vector<vector<double> > downMsg(20);
		vector<vector<int> > scopeOfUpMsg(20);
		vector<vector<double> > beliefOfCliqueNodes(20);
		vector<vector<int> > maxUpMsg(20);

		vector<int> upNbr(20,-1);
		vector<vector<int> > downNbr(20);

		for(int i=0;i<n1+n2;i++){

			for(int j=0;j<n1+n2;j++){

				adjacency[i][j] = 0;
			}
		}

		if(modelNum!=1){
			// adding trans edge

			for(int i=0;i<n1-1;i++){

				adjacency[i][i+1] = 1;
				adjacency[i+1][i] = 1;

			}

			for(int i=0;i<n2-1;i++){

				adjacency[n1+i][n1+i+1] = 1;
				adjacency[n1+i+1][n1+i] = 1;
				
			}
		}
		

		if(modelNum!=1 && modelNum!=2){
		// skip factor edge

			for(int i=0;i<n1-1;i++){
				for(int j=i+1;j<n1;j++){

					if(a1[i]==a1[j]){
						adjacency[i][j] = 1;
						adjacency[j][i] = 1;

					}
				}
			}

			for(int i=0;i<n2-1;i++){
				for(int j=i+1;j<n2;j++){

					if(a2[i]==a2[j]){
						adjacency[n1+i][n1+j] = 1;
						adjacency[n1+j][n1+i] = 1;
						
					}
				}
			}

		}

		if(modelNum == 4){
		// word - skip factor

			for(int i=0;i<n1;i++){

				for(int j=0;j<n2;j++){

					if(a1[i]==a2[j]){

						adjacency[i][n1+j] = 1;
						adjacency[n1+j][i] = 1;

					}
				}
			}
		}
		
		
		for(int i=0;i<totalVar;i++)
			for(int j=0;j<totalVar;j++)
				tempAdjacency[i][j] = adjacency[i][j];

		// finding elimination ordering using min-fill algorithm
		
		vector<int> a;

		for(int i=0;i<totalVar;i++)
			v[i] = 0;

		int countOfElimination = 0;

		while(countOfElimination != totalVar){
			int temp = INT_MAX, index;
			int t;
			for(int i=0;i<totalVar;i++){
				if(v[i]==0){
					t = calEdgeAdded(i);
					//cout<<t<<" ";
					if(t<temp){
						temp = t;
						index = i;
					}
				}


			}
			//cout<<"\n";
			a.clear();

			for(int i=0;i<totalVar;i++){

				if(tempAdjacency[index][i]==1 && v[i]==0)
					a.push_back(i);
			}

			int l = a.size();

			for(int i=0;i<l-1;i++){

				for(int j=0;j<l;j++){

					if(tempAdjacency[a[i]][a[j]]==0){
						tempAdjacency[a[i]][a[j]] = 1;
					}
				}
			}
			ordering[countOfElimination] = index;
			v[index] = 1;
			countOfElimination++;
		}
		
		
						
		for(int j=0;j<totalVar;j++){
			for(int k=0;k<totalVar;k++)
				cliqueAdjacency[j][k]=0;
		}		
		// Insert OCR factor Node 10*1 because Image Id is fixed
		
		// clique numbering from 0 

		// create clique tree
		for(int i=0;i<totalVar;i++)
			vEliminated[i] = 0;


		vector<int> p;

		int pSize;
		int fSize;
		int newFSize;
		


		for(int i=0;i<totalVar;i++){
			p.clear();
				
			class node *temp3 = new node;
			
			
			vEliminated[ordering[i]] = 1;
			//cout<<"Ordering"<<ordering[i]<<"\n";
			for(int j=0;j<totalVar;j++){

				if(adjacency[ordering[i]][j] == 1 && vEliminated[j]==0)
					p.push_back(j);

			}
			
			// check for factors which are created newly
			newFSize = scopeOfNewFactor.size();
			//cout<<newFSize<<" "<<i<<"\n";
			for(int k=0;k<newFSize;k++){

				int ll = scopeOfNewFactor[k].size();
				/*for(int ll1 = 0;ll1<ll;ll1++)
					cout<<scopeOfNewFactor[k][ll1]<<" ";
				cout<<"\n";
				*/
				if(visitedNewFactor[k]==0){

					if(find(scopeOfNewFactor[k].begin(),scopeOfNewFactor[k].end(),ordering[i])!=scopeOfNewFactor[k].end()){
						//cout<<"here\n";
						int m = scopeOfNewFactor[k].size();
						for(int l=0;l<m;l++){

							if(find(p.begin(),p.end(),scopeOfNewFactor[k][l])==p.end() && scopeOfNewFactor[k][l]!=ordering[i])
								p.push_back(scopeOfNewFactor[k][l]);
						}

						visitedNewFactor[k] = 1;	// removing that factor
						cliqueAdjacency[i][sourceNewFactor[k]] = 1;	// adding edges in clique tree
						cliqueAdjacency[sourceNewFactor[k]][i] = 1;
					}
				}
			}

			pSize = p.size();
			if(pSize != 0){

				// a new msg created
				sourceNewFactor.push_back(i);
				scopeOfNewFactor.push_back(p);
				visitedNewFactor.push_back(0);

			}
			
			
			temp3->scope.push_back(ordering[i]);

			for(int j=0;j<pSize;j++)
				temp3->scope.push_back(p[j]);


			pSize++;
			p.clear();
			for(int j=0;j<pSize;j++){
				p.push_back(temp3->scope[j]);
			}

			fSize = pow(10,pSize);

			temp3->factorSize = fSize;
			// Initializing ocrFactor

			int q=0;
			int imageId;

			imageId = a2[ordering[i]];

			vector<int> in(pSize);
			for(int k=0;k<pSize;k++)
				in[k] = 0;

			for(int k=0;k<fSize;k++){

				temp3->factorIndex.push_back(in);
				in[pSize-1]++;

				for(int l=pSize-1;l>0;l--){

					if(in[l]==10){
						in[l-1]++;
						in[l] = 0;
					}


				}

			}
			//cout<<fSize<<"\n";
			// updating OCR potentials, it's always on first column of index
			for(int k=0;k<fSize;k++){
				//cout<<ocrFactor[imageId][temp3->factorIndex[k][0]]<<"\n";
				temp3->factor.push_back(ocrFactor[imageId][temp3->factorIndex[k][0]]);
				//cout<<imageId<<" "<<temp3->factorIndex[k][0]<<" "<<exp(ocrFactor[imageId][temp3->factorIndex[k][0]])<<"\n";
				
			}
			// updating transFactor

			
			//cout<<"here2 "<<i<<"\n";
			if(modelNum!=1){
			
				int tempTrans = -1;
				if(ordering[i]!=n1-1 && ordering[i]!=totalVar-1 && vEliminated[ordering[i]+1]==0){
					for(int k=0;k<pSize;k++){

						if(p[k]==ordering[i]+1){

							tempTrans = k;
							break;

						}

					}
					
					if(tempTrans != -1){
						for(int k=0;k<fSize;k++){
							temp3->factor[k] += transFactor[temp3->factorIndex[k][0]][temp3->factorIndex[k][tempTrans]];
							//cout<<temp3->factor[k]<<" "<<temp3->factorIndex[k][0]<<" "<<temp3->factorIndex[k][tempTrans]<<"\n";

						}
					}
				}
				
				
				tempTrans = -1;
				if(ordering[i]!=0 && ordering[i]!=n1 && vEliminated[ordering[i]-1]==0){
					
					for(int k=0;k<pSize;k++){

						if(p[k]==ordering[i]-1){

							tempTrans = k;
							break;

						}

					}
					//cout<<tempTrans<<"\n";
					if(tempTrans != -1){
						for(int k=0;k<fSize;k++){
							temp3->factor[k] += transFactor[temp3->factorIndex[k][tempTrans]][temp3->factorIndex[k][0]];
							//cout<<temp3->factor[k]<<" "<<temp3->factorIndex[k][tempTrans]<<" "<<temp3->factorIndex[k][0]<<" "<<transFactor[temp3->factorIndex[k][tempTrans]][temp3->factorIndex[k][0]]<<"\n";

						}
					}
				}
			}

		
			// updating skip factor and word-skip

			

				// updating skip
				int tempImg1 = a2[ordering[i]];

				if(modelNum!=1 && modelNum!=2){
					for(int k=0;k<n2;k++){
						int tempSkip1;
						if(k!=ordering[i] && tempImg1 == a2[k] && vEliminated[k]==0){
							// same Image Id, adding skip factors
							
							for(int l=0;l<pSize;l++){
								if(p[l]==k){
									tempSkip1 = l;
									break;
								}

							}

							// add skip factor 

							for(int l=0;l<fSize;l++){

								if(temp3->factorIndex[l][0]==temp3->factorIndex[l][tempSkip1]){
									temp3->factor[l] += log(5);
								}else{
									temp3->factor[l] +=log(1);

								}
							}

						}


					}
				}

				if(modelNum==4){
				//updating word skip
					for(int k=n1;k<totalVar;k++){
						int tempSkip1;
						if( tempImg1 == a2[k-n1] && vEliminated[k]==0){


							for(int l=0;l<pSize;l++){
								if(p[l]==k){
									tempSkip1 = l;
									break;
								}

							}

							// add word-skip factor

							for(int l=0;l<fSize;l++){

								if(temp3->factorIndex[l][0] == temp3->factorIndex[l][tempSkip1]){

									temp3->factor[l] += log(5);

								}else{

									temp3->factor[l] += log(1);

								}

							}


						}


					}
				}


			

			
			cliques[i] = temp3;
			
		}

		totalVar = n2;

		int root = totalVar-1;

		for(int j=0;j<totalVar;j++)
			upNbr.push_back(-1);

		int vEle[totalVar];
		for(int j=0;j<totalVar;j++)
			vEle[j] =0;

		queue<int> q;


		q.push(root);

		vEle[totalVar-1] = 1;
		
		while(!q.empty()){

			int tempRoot = q.front();
			q.pop();
			

			for(int j=0;j<totalVar;j++){

				if(j!=tempRoot && vEle[j]==0 && cliqueAdjacency[tempRoot][j]==1){
					//cout<<j<<"\n";
					//cout<<j<<endl;
					upNbr[j] = tempRoot;
					vEle[j] = 1;
					q.push(j);
				}
			}


		}

		
		root = totalVar-1;


		q.push(root);
		
		for(int i=0;i<totalVar;i++)
			vEle[i] = 0;
		vEle[root] = 1;

		

		while(!q.empty()){

			int tempRoot = q.front();
			q.pop();

			for(int j=0;j<totalVar;j++){

				if(j!=tempRoot && vEle[j]==0 && cliqueAdjacency[tempRoot][j]==1){
						downNbr[tempRoot].push_back(j);
						vEle[j] = 1;
						q.push(j);
				}
			}

		}

		// two-way message passing
		// upward pass

		vector<int> countUpMsg(totalVar,0);

		int downNbrRoot = downNbr[totalVar-1].size();
		int vUp[totalVar];
		for(int i=0;i<totalVar-1;i++)
			vUp[i] = 0;


		
		while(downNbrRoot != countUpMsg[totalVar-1]){

			for(int i=0;i<totalVar-1;i++){
				int tempSize = downNbr[i].size();
			
				if(tempSize == countUpMsg[i] && vUp[i]==0){
					vector<double> tempMsg;
					vector<int> tempScope;
					vUp[i] = 1;
					int j = upNbr[i];
					countUpMsg[j]++;
				
					up_MSG(i, downNbr[i], cliques, tempScope, scopeOfUpMsg, tempMsg, upMsg);
					//cout<<"here3\n";
					upMsg[i] = tempMsg;
					scopeOfUpMsg[i] = tempScope;
					
				}
			}


		}

		int upMsgPresent[totalVar][totalVar];
		int downMsgPresent[totalVar][totalVar];
		int countDownMsg = 0;

		for(int i=0;i<totalVar;i++){

			for(int j=0;j<totalVar;j++){
				
				upMsgPresent[i][j] = 0;
				downMsgPresent[j][i] = 0;

				if(upNbr[i]==j){

					upMsgPresent[i][j] = 1;
					downMsgPresent[j][i] = 1;
					countDownMsg++;

				}


			}
		}
		
		
		
		int vDown[totalVar];
		for(int i=0;i<totalVar;i++)
			vDown[i] = 0;
		vector<double> downFactor;
		vector<double> beliefFactor;
		map<string,double> mapping;
		countDownMsg = totalVar;

		while(countDownMsg!=0){
			
			for(int i=0;i<totalVar;i++){

				if((upNbr[i]==-1 || vDown[upNbr[i]] == 1 )&& vDown[i]==0){
					//cout<<"here\n";
					
					countDownMsg--;
					vDown[i] = 1;
					
					class node *tempNode = cliques[i];

					int tempDownNbrSize = downNbr[i].size();

					
					tempMsg.clear();
					tempScope.clear();


					createBelief(i, cliques, scopeOfUpMsg, upNbr, downMsg, tempMsg, downNbr, upMsg, beliefOfCliqueNodes);

					for(int j=0;j<tempDownNbrSize;j++){

						// send msg to down nbr j
						// divide by factor coming from downNr[i][j]
						beliefFactor = beliefOfCliqueNodes[i];


						downFactor = upMsg[downNbr[i][j]];
						int tempSize = scopeOfUpMsg[downNbr[i][j]].size();
						int newTableSize = pow(10,tempSize);
						int scopeSize = tempNode->scope.size();
						int fSize = pow(10,scopeSize);

						vector<int> in(tempSize);

						for(int k=0;k<tempSize;k++)
							in[k] = 0;

						for(int k=0;k<newTableSize;k++){

							string s = "";
							for(int z=0;z<tempSize;z++)
								s += to_string(in[z]);

							mapping[s] = downFactor[k];

							in[tempSize-1]++;

							for(int l=tempSize-1;l>0;l--){

								if(in[l]==10){
									in[l-1]++;
									in[l] = 0;
								}


							}

						}

						vector<int> order;

						

						for(int k=0;k<tempSize;k++){
							for(int j1=0;j1<scopeSize;j1++){

								if(scopeOfUpMsg[downNbr[i][j]][k] == tempNode->scope[j1]){
									order.push_back(j1);
									break;

								}
							}

						}

						for(int l=0;l<fSize;l++){

								string tempString = "";
								for(int z=0;z<tempSize;z++){
									
									tempString += to_string(tempNode->factorIndex[l][order[z]]);
								}


								beliefFactor[l] -= mapping[tempString];
								
						}
					
						mapping.clear();

						for(int k=0;k<tempSize;k++)
							in[k] = 0;

						for(int k=0;k<newTableSize;k++){

							string s = "";
							for(int z=0;z<tempSize;z++)
								s += to_string(in[z]);

							mapping[s] = 0.0;
							

							in[tempSize-1]++;

							for(int l=tempSize-1;l>0;l--){

								if(in[l]==10){
									in[l-1]++;
									in[l] = 0;
								}


							}

						}
						

						for(int l=0;l<fSize;l++){

								string tempString = "";
								for(int z=0;z<tempSize;z++){
									
									tempString += to_string(tempNode->factorIndex[l][order[z]]);
								}
								
								mapping[tempString] += exp(beliefFactor[l]);

						}

						for(int k=0;k<tempSize;k++)
							in[k] = 0;


						for(int k=0;k<newTableSize;k++){

							string s = "";
							for(int z=0;z<tempSize;z++)
								s += to_string(in[z]);

							downFactor[k] = log(mapping[s]);
							

							in[tempSize-1]++;

							for(int l=tempSize-1;l>0;l--){

								if(in[l]==10){
									in[l-1]++;
									in[l] = 0;
								}
							}

						}

						// msg created

						downMsg[downNbr[i][j]] = downFactor;

						downFactor.clear();
					}		
				}
			}
		}


		int assignment[10];


		for(int i=0;i<totalVar-1;i++){

			double marginalBelief[10];

			for(int j=0;j<10;j++)
				marginalBelief[j] = 0.0;

			int count= 0;

			int remainingTableSize = upMsg[i].size();

			int fSize = cliques[i]->factorSize;


			vector<double> factor = beliefOfCliqueNodes[i];

			int startIndex  = 0;

			for(int j=0;j<10;j++){


				int endIndex = startIndex + remainingTableSize;

				for(int k= startIndex;k<endIndex;k++){

					marginalBelief[j] += exp(factor[k]);
				}
				marginalBelief[j] = log(marginalBelief[j]);

				startIndex = startIndex + remainingTableSize;
			}

			double maxValue = exp(marginalBelief[0]);
			int ass = 0;
			double z = maxValue;

			for(int i1=1;i1<10;i1++){

				double potentialMax = exp(marginalBelief[i1]);
				z+= potentialMax;

				if(potentialMax > maxValue){

					maxValue = potentialMax;
					ass = i1;
				}
			}

			double num = exp(marginalBelief[a4[i]]);

			num /=z;

			DLL += log(num); 


			assignment[ordering[i]] = ass;

		}

		vector<double> factor = beliefOfCliqueNodes[totalVar-1];
		double maxValue = exp(factor[0]);
		int ass = 0;

		double z = maxValue;

		for(int i=1;i<10;i++){

			double potentialMax = exp(factor[i]);
			z += potentialMax;
			if(potentialMax > maxValue){

				maxValue = potentialMax;
				ass = i;
			}

		}

		double num = exp(factor[a4[ordering[totalVar-1]]]);

		num /= z;

		DLL += log(num);


		assignment[ordering[totalVar-1]] = ass;

		int prevcharCount = characterCounter;

		for(int i=0;i<n2;i++){

			if(assignment[i] == a4[i])
				characterCounter++;
			totalChar++;
		}

		if((characterCounter - prevcharCount) == n2)
				correctWord++;

	}
		



		}
		else{

		for(int i=0;i<totalVar;i++){

			class node *tempNode = cliques[i];

			// print scope 

			int scopeSize = tempNode->scope.size();

			for(int i=0;i<scopeSize;i++)
				cout<<tempNode->scope[i]<<"   ";

			cout<<"\n\n";

			// print factor
			int sizeFactor = tempNode->factor.size();

			for(int i=0;i<sizeFactor;i++){
				cout<<tempNode->factor[i]<<"    ";
			}
			cout<<"\n\n\n\n\n\n";

		}
		exit(0);

		
		int root = totalVar - 1;
		//vector<int> upNbr(totalVar,-1);
		//vector<vector<int> > downNbr(totalVar);
		for(int j=0;j<totalVar;j++)
			upNbr.push_back(-1);

		int vEle[totalVar];
		for(int j=0;j<totalVar;j++)
			vEle[j] =0;

		queue<int> q;

		q.push(root);

		vEle[totalVar-1] = 1;
		
	
		while(!q.empty()){

			int tempRoot = q.front();
			q.pop();
			//cout<<tempRoot<<endl;

			for(int j=0;j<totalVar;j++){

				if(j!=tempRoot && vEle[j]==0 && cliqueAdjacency[tempRoot][j]==1){
					//cout<<j<<endl;
					upNbr[j] = tempRoot;
					vEle[j] = 1;
					q.push(j);
				}
			}


		}

		root = totalVar-1;
		q.push(root);
		
		for(int i=0;i<totalVar;i++)
			vEle[i] = 0;
		vEle[root] = 1;

		while(!q.empty()){

			int tempRoot = q.front();
			q.pop();

			for(int j=0;j<totalVar;j++){

				if(j!=tempRoot && vEle[j]==0 && cliqueAdjacency[tempRoot][j]==1){
						downNbr[tempRoot].push_back(j);
						vEle[j] = 1;
						q.push(j);
				}
			}

		}

		// two-way message passing

		vector<int> countUpMsg(totalVar,0);

		int downNbrRoot = downNbr[totalVar-1].size();
		int vUp[totalVar];
		for(int i=0;i<totalVar-1;i++)
			vUp[i] = 0;


		
		while(downNbrRoot != countUpMsg[totalVar-1]){

			for(int i=0;i<totalVar-1;i++){
				int tempSize = downNbr[i].size();
				
				if(tempSize == countUpMsg[i] && vUp[i]==0){
					vector<double> tempMsg;
					vector<int> tempScope;
					vUp[i] = 1;
					int j = upNbr[i];
					countUpMsg[j]++;
					


					
					up_MSG(i, downNbr[i], cliques, tempScope, scopeOfUpMsg, tempMsg, upMsg);
					//cout<<"here3\n";
					upMsg[i] = tempMsg;
					scopeOfUpMsg[i] = tempScope;
					
				}
			}


		}

		int upMsgPresent[totalVar][totalVar];
		int downMsgPresent[totalVar][totalVar];
		int countDownMsg = 0;

		for(int i=0;i<totalVar;i++){

			for(int j=0;j<totalVar;j++){
				
				upMsgPresent[i][j] = 0;
				downMsgPresent[j][i] = 0;

				if(upNbr[i]==j){

					upMsgPresent[i][j] = 1;
					downMsgPresent[j][i] = 1;
					countDownMsg++;

				}
			}
		}
		
		
		
		int vDown[totalVar];
		for(int i=0;i<totalVar;i++)
			vDown[i] = 0;
		vector<double> downFactor;
		vector<double> beliefFactor;
		map<string,double> mapping;
		countDownMsg = totalVar;

		while(countDownMsg!=0){
			
			for(int i=0;i<totalVar;i++){

				if((upNbr[i]==-1 || vDown[upNbr[i]] == 1 )&& vDown[i]==0){
					//cout<<"here\n";
					
					countDownMsg--;
					vDown[i] = 1;
					
					class node *tempNode = cliques[i];

					int tempDownNbrSize = downNbr[i].size();

					
					tempMsg.clear();
					tempScope.clear();


					createBelief(i, cliques, scopeOfUpMsg, upNbr, downMsg, tempMsg, downNbr, upMsg, beliefOfCliqueNodes);

					for(int j=0;j<tempDownNbrSize;j++){

						// send msg to down nbr j
						// divide by factor coming from downNr[i][j]
						beliefFactor = beliefOfCliqueNodes[i];


						downFactor = upMsg[downNbr[i][j]];
						int tempSize = scopeOfUpMsg[downNbr[i][j]].size();
						int newTableSize = pow(10,tempSize);
						int scopeSize = tempNode->scope.size();
						int fSize = pow(10,scopeSize);

						vector<int> in(tempSize);

						for(int k=0;k<tempSize;k++)
							in[k] = 0;

						for(int k=0;k<newTableSize;k++){

							string s = "";
							for(int z=0;z<tempSize;z++)
								s += to_string(in[z]);

							mapping[s] = downFactor[k];

							in[tempSize-1]++;

							for(int l=tempSize-1;l>0;l--){

								if(in[l]==10){
									in[l-1]++;
									in[l] = 0;
								}


							}

						}

						vector<int> order;

						

						for(int k=0;k<tempSize;k++){
							for(int j1=0;j1<scopeSize;j1++){

								if(scopeOfUpMsg[downNbr[i][j]][k] == tempNode->scope[j1]){
									order.push_back(j1);
									break;

								}
							}

						}

						for(int l=0;l<fSize;l++){

								string tempString = "";
								for(int z=0;z<tempSize;z++){
									
									tempString += to_string(tempNode->factorIndex[l][order[z]]);
								}


								beliefFactor[l] -= mapping[tempString];
								
						}
					
						mapping.clear();

						for(int k=0;k<tempSize;k++)
							in[k] = 0;

						for(int k=0;k<newTableSize;k++){

							string s = "";
							for(int z=0;z<tempSize;z++)
								s += to_string(in[z]);

							mapping[s] = 0.0;
							

							in[tempSize-1]++;

							for(int l=tempSize-1;l>0;l--){

								if(in[l]==10){
									in[l-1]++;
									in[l] = 0;
								}


							}

						}
					

						for(int l=0;l<fSize;l++){

								string tempString = "";
								for(int z=0;z<tempSize;z++){
									
									tempString += to_string(tempNode->factorIndex[l][order[z]]);
								}

								mapping[tempString] += exp(beliefFactor[l]);

						}

						for(int k=0;k<tempSize;k++)
							in[k] = 0;


						for(int k=0;k<newTableSize;k++){

							string s = "";
							for(int z=0;z<tempSize;z++)
								s += to_string(in[z]);

							downFactor[k] = log(mapping[s]);
							

							in[tempSize-1]++;

							for(int l=tempSize-1;l>0;l--){

								if(in[l]==10){
									in[l-1]++;
									in[l] = 0;
								}
							}

						}

						// msg created

						downMsg[downNbr[i][j]] = downFactor;

						downFactor.clear();

					}

				}

			}
		
		}

		int assignment[10];


		for(int i=0;i<totalVar-1;i++){

			double marginalBelief[10];

			for(int j=0;j<10;j++)
				marginalBelief[j] = 0.0;

			int count= 0;

			int remainingTableSize = upMsg[i].size();

			int fSize = cliques[i]->factorSize;


			vector<double> factor = beliefOfCliqueNodes[i];

			int startIndex  = 0;

			for(int j=0;j<10;j++){


				int endIndex = startIndex + remainingTableSize;

				for(int k= startIndex;k<endIndex;k++){

					marginalBelief[j] += exp(factor[k]);
				}
				marginalBelief[j] = log(marginalBelief[j]);

				startIndex = startIndex + remainingTableSize;
			}

			double maxValue = exp(marginalBelief[0]);
			int ass = 0;

			double z = maxValue;

			for(int i=1;i<10;i++){

				double potentialMax = exp(marginalBelief[i]);

				z += potentialMax;

				if(potentialMax > maxValue){

					maxValue = potentialMax;
					ass = i;
				}
			}

			if(ordering[i] < n1){

				double num = exp(marginalBelief[a3[ordering[i]]]);
				num /=z;
				DLL += log(num);
			}else{

				double num = exp(marginalBelief[a4[ordering[i]-n1]]);
				num /=z;
				DLL += log(num);

			}

			assignment[ordering[i]] = ass;

		}

		vector<double> factor = beliefOfCliqueNodes[totalVar-1];
		double maxValue = exp(factor[0]);

		double z  = maxValue;
		int ass = 0;

		for(int i=1;i<10;i++){

			double potentialMax = exp(factor[i]);
			z+= potentialMax;
			if(potentialMax > maxValue){

				maxValue = potentialMax;
				ass = i;
			}

		}

		if(ordering[totalVar-1]< n1){

			double num = exp(factor[a3[ordering[totalVar-1]]]);
			num /=z;
			DLL += log(num);
		}else{

			double num = exp(factor[a4[ordering[totalVar-1]-n1]]);
			num /=z;
			DLL += log(num);
		}

		assignment[ordering[totalVar-1]] = ass;

		//for(int i=0;i<totalVar;i++)
		//	cout<<assignment[i]<<" ";
		int prevcharCount = characterCounter;
		for(int i=0;i<n1;i++){

			if(assignment[i] == a3[i])
				characterCounter++;
			totalChar++;
		}

		if((characterCounter - prevcharCount) == n1)
				correctWord++;

		prevcharCount = characterCounter;
		for(int i=0;i<n2;i++){

			if(assignment[n1+i] == a4[i])
				characterCounter++;
			totalChar++;
		}
		if((characterCounter - prevcharCount) == n2)
				correctWord++;



		}

		
	}
	ccounter--;
	cout<<"Character Count :"<<characterCounter<<" "<<totalChar<<"\n";
	double percent = characterCounter/double(totalChar);
	cout<<"percent :"<<percent<<"\n";
	double value = correctWord/double(2*ccounter);
	cout<<value<<"\n";
	value = DLL/double(2* ccounter);
	cout<<value<<"\n";

}
