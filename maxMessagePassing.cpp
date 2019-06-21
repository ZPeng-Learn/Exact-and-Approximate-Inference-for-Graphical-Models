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

class node{

public:
	

	vector<double> factor;

	int factorSize;

	vector<int> scope;

	vector<vector<int> > factorIndex;

	

};


int correctWord = 0;
void up_MSG(int index, vector<int>& child, map<int, class node * > &cliques, vector<int> &tempScope, vector<vector<int> > &scopeOfUpMsg, vector<double> &tempMsg, vector<vector<int> > &maxUpMsg, vector<vector<double> > &upMsg, vector<vector<double> > &values){




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
				//cout<<"imp\n";
				//cout<<scopeOfUpMsg[child[i]][l]<<" "<<tempNode->scope[z]<<"\n";
				if(scopeOfUpMsg[child[i]][l] == tempNode->scope[z]){
					order.push_back(z);
					break;
				}
			}
		}


		//cout<<index<<"\n";
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

		tempValue = exp(tempFactor[i]);
		int tempIndex = 0;
		int counter = 0;
		for(int j=i+newTableSize;j<tempNode->factorSize;j = j + newTableSize){
			counter++;
			//cout<<tempValue<<" "<<
			double potentialMax = exp(tempFactor[j]);
			if(tempValue < potentialMax){
				tempValue = potentialMax;
				tempIndex = counter;
				
			}
		}
		tempValue = log(tempValue);
		maxUpMsg[index].push_back(tempIndex);

		tempMsg.push_back(tempValue);

	}

		// for DLL
	for(int i=0;i<10;i++){

		int start = i*newTableSize;

		double temp = 0.0;

		for(int j=start;j<start+newTableSize;j++){

			temp += exp(tempFactor[j]);
		}

		values[index].push_back(temp);

	}


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

int totalChar=0;


int main(){


	ifstream file;
	ifstream fileOCR;
	ifstream fileTrans;
	ifstream fileTruth;
	string temp;
	int a1[100],a2[100];

	char temp1[1000];

	char *c;


	int modelNum;

	cin>>modelNum;

	file.open("./OCRdataset-2/data/data-loopsWS.dat");
	fileOCR.open("./OCRdataset-2/potentials/ocr.dat");
	fileTrans.open("./OCRdataset-2/potentials/trans.dat");
	fileTruth.open("./OCRdataset-2/data/truth-loopsWS.dat");

	double DLL =0.0;

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

	while(!file.eof()){

		 n1  = 0, n2  = 0;

		ccounter++;
		cout<<ccounter<<"\n";

		map<int, class node *> cliques;
		vector<vector<int> > scopeOfNewFactor;
		vector<int> visitedNewFactor;
		vector<int> sourceNewFactor;
		int cliqueAdjacency[100][100];

		int transAdded[100];
		

		vector<vector<double> > upMsg(20);
		vector<vector<int> > scopeOfUpMsg(20);
		vector<vector<double> > beliefOfCliqueNodes(20);
		vector<vector<int> > maxUpMsg(20);

		vector<int> upNbr(20,-1);
		vector<vector<int> > downNbr(20);
		vector<vector<double> > values(20);


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
		
		//cout<<temp<<"\n";

		
		c = strtok(temp1,"\t");
		while(c!=NULL){

			a1[n1++] = atoi(c);
			
			c  = strtok(NULL,"\t");
			

		}
	
		for(int i=0;i<totalVar;i++)
			transAdded[i] = 0;

		getline(file,temp);
		strcpy(temp1, temp.c_str());

		//cout<<temp<<"\n";

		c = strtok(temp1,"\t");
		while(c!=NULL){

			a2[n2++] = atoi(c);
			
			c  = strtok(NULL,"\t");
			

		}
		
		totalVar = n1 + n2;


		getline(file,temp);
		getline(fileTruth,temp);
		
		for(int i=0;i<n1+n2;i++){

			for(int j=0;j<n1+n2;j++){

				adjacency[i][j] = 0;
			}
		}


		// adding trans edge
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
		

		if(modelNum!=1 && modelNum!=2 && modelNum !=3){
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
			/*
			for(int l=0;l<p.size();l++){
				cout<<p[l]<<" ";
			}
			cout<<"\n";
			*/
			// check for factors which are created newly
			newFSize = scopeOfNewFactor.size();
			for(int k=0;k<newFSize;k++){

				int ll = scopeOfNewFactor[k].size();
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
			
			// updating OCR potentials, it's always on first column of index
			for(int k=0;k<fSize;k++){
				
				temp3->factor.push_back(ocrFactor[imageId][temp3->factorIndex[k][0]]);
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

						}
					}
				}
			}

			// updating skip factor and word-skip

			if(ordering[i]<n1){

				// updating skip
				int tempImg1 = a1[ordering[i]];

				if(modelNum!=1 && modelNum!=2 && modelNum!=3){
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

				//cout<<"hhhh\n";
				int tempImg1 = a2[ordering[i] - n1];

				if(modelNum!=1 && modelNum!=2 && modelNum!=3){

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
					//cout<<maxValue<<"\n";

					double z = maxValue;
					for(int j=1;j<fSize;j++){
						double potentialMax = exp(tempNode->factor[j]);
						//cout<<potentialMax<<"\n";
						z += potentialMax;



						if(maxValue < potentialMax){

							maxValue = potentialMax;
							maxIndex = j;
							
						}
						
					}

					if(ordering[i] < n1){

						double num = exp(tempNode->factor[a3[ordering[i]]]);
						num /=z;
						DLL += log(num);


					}else{

						double num = exp(tempNode->factor[a4[ordering[i]-n1]]);
						num /=z;
						DLL += log(num);

					}

					assignment[ordering[i]] = maxIndex;

				}

				int prevCharCount = characterCounter;
				for(int i=0;i<n1;i++){

					if(assignment[i] == a3[i])
						characterCounter++;
					totalChar++;
				}

				if((characterCounter - prevCharCount) == n1)
					correctWord++;

				int prevcharCount = characterCounter;
				for(int i=0;i<n2;i++){

					if(assignment[n1+i] == a4[i])
						characterCounter++;
					totalChar++;
				}

				if((characterCounter - prevcharCount) == n2)
					correctWord++;

			}else if(modelNum ==2 || modelNum == 3){




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
					//cout<<tempSize <<" "<<countUpMsg[i]<< " "<<vUp[i]<<"\n"; 
					//if(tempSize == 1)
					//	cout<<downNbr[i][0]<<"\n";
					if(tempSize == countUpMsg[i] && vUp[i]==0){
						vector<double> tempMsg;
						vector<int> tempScope;
						vUp[i] = 1;
						int j = upNbr[i];
						countUpMsg[j]++;
					
						up_MSG(i, downNbr[i], cliques, tempScope, scopeOfUpMsg, tempMsg, maxUpMsg, upMsg, values);
						//cout<<"here3\n";
						upMsg[i] = tempMsg;
						scopeOfUpMsg[i] = tempScope;
						
					}
				}


			}
			
			
			vector<int> assignment(totalVar);
			int vAss[totalVar];
			vAss[totalVar-1] = 1;
			class node *root1 = cliques[totalVar-1];

			for(int i=0;i<totalVar-1;i++)
				vAss[i] = 0;
			
			double finalTable[10];

			for(int i=0;i<10;i++)
				finalTable[i] = root1->factor[i];

			for(int i=0;i<downNbrRoot;i++){

				for(int j=0;j<10;j++){

					finalTable[j] += upMsg[downNbr[totalVar-1][i]][j];
					//cout<<finalTable[j]<<" ";
				}


			}
			

			for(int i=0;i<totalVar-1;i++){


				int l = upMsg[i].size();
				
				class node *tempNode = cliques[i];

				int ll = tempNode->scope.size();

				for(int j=0;j<ll;j++){
					cout<<tempNode->scope[j]<<" ";
				}
				cout<<"\n";

				for(int j=0;j<l;j++)
					cout<<upMsg[i][j]<<" ";

				cout<<"\n";
			}
			
			int maxIndex = 0;
			double maxValue = exp(finalTable[0]);
			double z  = maxValue;
			cout<<finalTable[0]<<" ";
			for(int i=1;i<10;i++){
				cout<<finalTable[i]<<" ";
				double potentialMax = exp(finalTable[i]);
				z += potentialMax;
				if(maxValue < potentialMax){

					maxValue = potentialMax;
					maxIndex = i;

				}
			} 
			cout<<"\n";

			double num = exp(finalTable[a3[totalVar-1]]);

			num /=z;

			DLL += log(num);
			
			// assignment to element eliminated in last
			assignment[totalVar-1] = maxIndex;

			for(int i=totalVar-2;i>=0;i--){

				string ss = "";

				int ll = scopeOfUpMsg[i].size();

				if(ll==0){

					


				}else{

					for(int j=0;j<ll;j++){
						int ttemp;
						for(int k=i+1;k<totalVar;k++){
							if(ordering[k] == scopeOfUpMsg[i][j]){
								ttemp = k;break;

							}

						}

						ss += to_string(assignment[ttemp]);

					}

					

					vector<int> in(ll);
					for(int k=0;k<ll;k++)
						in[k] = 0;
					int newTableSize = pow(10, ll);
					
					for(int k=0;k<newTableSize;k++){

						string s = "";
						for(int z=0;z<ll;z++)
							s += to_string(in[z]);

						if(s.compare(ss)==0){

							//cout<<maxUpMsg[i][k]<<" ";
							assignment[i] = maxUpMsg[i][k];
							break;
						}


						in[ll-1]++;

						for(int l=ll-1;l>0;l--){

							if(in[l]==10){
								in[l-1]++;
								in[l] = 0;
							}


						}

					}

				}


			}
			
			for(int i=0;i<totalVar-1;i++){

				double z = 0.0;
				for(int j=0;j<10;j++){

					z += values[i][j];


				}

				double num = values[i][a3[ordering[i]]];
				num /=z;

				DLL += log(num);

			}

			int prevcharCount = characterCounter;
			
			for(int i=0;i<totalVar;i++){


				if(ordering[i] >= n1){

					if(a4[ordering[i] - n1] == assignment[i])
						characterCounter++;
					totalChar++;


				}else{

					if(a3[ordering[i]] == assignment[i])
						characterCounter++;
					totalChar++;
				}
			}

			if((characterCounter - prevcharCount) == n1)
				correctWord++;


			



			if(n2>0){


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
		vector<vector<double> > values(20);

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
			
			// updating OCR potentials, it's always on first column of index
			for(int k=0;k<fSize;k++){
				temp3->factor.push_back(ocrFactor[imageId][temp3->factorIndex[k][0]]);
			
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

				if(modelNum!=1 && modelNum!=2 && modelNum!=3){
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
						up_MSG(i, downNbr[i], cliques, tempScope, scopeOfUpMsg, tempMsg, maxUpMsg, upMsg, values);
						//cout<<"here3\n";
						upMsg[i] = tempMsg;
						scopeOfUpMsg[i] = tempScope;
					}
				}
			}
		
			vector<int> assignment(totalVar);
			int vAss[totalVar];
			vAss[totalVar-1] = 1;
			class node *root1 = cliques[totalVar-1];

			for(int i=0;i<totalVar-1;i++)
				vAss[i] = 0;
			
			double finalTable[10];

			for(int i=0;i<10;i++)
				finalTable[i] = root1->factor[i];

			for(int i=0;i<downNbrRoot;i++){

				for(int j=0;j<10;j++){

					finalTable[j] += upMsg[downNbr[totalVar-1][i]][j];
					//cout<<finalTable[j]<<" ";
				}
				//cout<<"\n";


			}
			
			int maxIndex = 0;
			double maxValue = exp(finalTable[0]);
			double z = maxValue;
			//cout<<finalTable[0]<<" ";
			for(int i=1;i<10;i++){
				//cout<<finalTable[i]<<" ";
				double potentialMax = exp(finalTable[i]);
				z += potentialMax;
				if(maxValue < potentialMax){

					maxValue = potentialMax;
					maxIndex = i;

				}
			} 
			//cout<<"\n";

			double num = exp(finalTable[a4[ordering[totalVar-1]]]);
			num /=z;

			DLL += log(num);
			
			// assignment to element eliminated in last
			assignment[totalVar-1] = maxIndex;

			for(int i=totalVar-2;i>=0;i--){

				string ss = "";

				int ll = scopeOfUpMsg[i].size();

				if(ll==0){

				}else{

					for(int j=0;j<ll;j++){
						int ttemp;
						for(int k=i+1;k<totalVar;k++){
							if(ordering[k] == scopeOfUpMsg[i][j]){
								ttemp = k;break;

							}

						}

						ss += to_string(assignment[ttemp]);

					}


					vector<int> in(ll);
					for(int k=0;k<ll;k++)
						in[k] = 0;
					int newTableSize = pow(10, ll);
					
					for(int k=0;k<newTableSize;k++){

						string s = "";
						for(int z=0;z<ll;z++)
							s += to_string(in[z]);

						if(s.compare(ss)==0){
							assignment[i] = maxUpMsg[i][k];
							break;
						}


						in[ll-1]++;

						for(int l=ll-1;l>0;l--){

							if(in[l]==10){
								in[l-1]++;
								in[l] = 0;
							}


						}
					}
				}
			}
			for(int i=0;i<totalVar-1;i++){

				double z = 0.0;
				for(int j=0;j<10;j++){

					z += values[i][j];
				}

				double num = values[i][a3[ordering[i]]];
				num /=z;

				DLL += log(num);

			}

			int prevcharCount = characterCounter;
			
			for(int i=0;i<totalVar;i++){
				
					if(a4[ordering[i]] == assignment[i])
						characterCounter++;
					totalChar++;

			}

			if((characterCounter - prevcharCount) == n2)
				correctWord++;

		}
			}
			else{
		
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
						up_MSG(i, downNbr[i], cliques, tempScope, scopeOfUpMsg, tempMsg, maxUpMsg, upMsg, values);
						//cout<<"here3\n";
						upMsg[i] = tempMsg;
						scopeOfUpMsg[i] = tempScope;
						
					}
				}
			}
			
			vector<int> assignment(totalVar);
			int vAss[totalVar];
			vAss[totalVar-1] = 1;
			class node *root1 = cliques[totalVar-1];

			for(int i=0;i<totalVar-1;i++)
				vAss[i] = 0;
			
			double finalTable[10];

			for(int i=0;i<10;i++)
				finalTable[i] = root1->factor[i];

			for(int i=0;i<downNbrRoot;i++){

				for(int j=0;j<10;j++){

					finalTable[j] += upMsg[downNbr[totalVar-1][i]][j];
					
				}
			
			}
			
			
			int maxIndex = 0;
			double maxValue = exp(finalTable[0]);
			//cout<<finalTable[0]<<" ";
			double z = maxValue;
			for(int i=1;i<10;i++){
				//cout<<finalTable[i]<<" ";

				double potentialMax = exp(finalTable[i]);
				z += potentialMax;
				if(maxValue < potentialMax){

					maxValue = potentialMax;
					maxIndex = i;

				}
			}

			if(ordering[totalVar-1] < n1){

				double num = exp(finalTable[a3[ordering[totalVar-1]]]);
				num /=z;
				DLL += log(num);

			}else{

				double num = exp(finalTable[a4[ordering[totalVar-1]-n1]]);
				num /=z;
				DLL += log(num);



			}
			// assignment to element eliminated in last
			assignment[totalVar-1] = maxIndex;

			for(int i=totalVar-2;i>=0;i--){

				string ss = "";

				int ll = scopeOfUpMsg[i].size();

				if(ll==0){
				}else{

					for(int j=0;j<ll;j++){
						int ttemp;
						for(int k=i+1;k<totalVar;k++){
							if(ordering[k] == scopeOfUpMsg[i][j]){
								ttemp = k;break;

							}

						}

						ss += to_string(assignment[ttemp]);

					}
					
					vector<int> in(ll);
					for(int k=0;k<ll;k++)
						in[k] = 0;
					int newTableSize = pow(10, ll);
					
					for(int k=0;k<newTableSize;k++){

						string s = "";
						for(int z=0;z<ll;z++)
							s += to_string(in[z]);

						if(s.compare(ss)==0){

							//cout<<maxUpMsg[i][k]<<" ";
							assignment[i] = maxUpMsg[i][k];
							break;
						}


						in[ll-1]++;

						for(int l=ll-1;l>0;l--){

							if(in[l]==10){
								in[l-1]++;
								in[l] = 0;
							}
						}

					}

				}

			}

			for(int i=0;i<totalVar-1;i++){
				double z =0.0;
				for(int j=0;j<10;j++){

					z += values[i][j];

				}

				if(ordering[i] < n1){

					double num = values[i][a3[ordering[i]]];
					num /=z ;
					DLL += log(num);


				}else{

					double num = values[i][a4[ordering[i]-n1]];
					num /=z ;
					DLL += log(num);



				}
			}

			//cout<<"Predicted : ";
			for(int i=0;i<totalVar;i++){

				cout<<ordering[i]<<" "<<assignment[i]<<"\n";
			}
			
			int correct3 = 0;
			int correct4 = 0;

			for(int i=0;i<totalVar;i++){


				if(ordering[i] >= n1){

					if(a4[ordering[i] - n1] == assignment[i]){
						characterCounter++;
						correct4++;

					}

					totalChar++;


				}else{

					if(a3[ordering[i]] == assignment[i]){
						characterCounter++;
						correct3++;
					}
					totalChar++;


				}
			}

			if(correct3==n1)
				correctWord++;
			if(correct4==n2)
				correctWord++;

			

			int ll = maxUpMsg[2].size();

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
