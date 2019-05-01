#include "readData.h"
#include "Util.h"
//#include "json.hpp"

#include <string>
#include <random>
#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <time.h>

using namespace std;
//using json = nlohmann::json;

typedef struct{
	int posicaoVertice1;
	int vertice1;
	int posicaoVertice2;
	int vertice2;
}tSwap;

typedef struct{
	int posicaoDoVertice;
	int vertice;
	int posicaoDeInsercao;
}tReinsercao;

typedef struct{
	int posicao;
	int vertice;
	int custo;
}tConstrutivo;

// Pega da instâncias
double ** matrizAdj; // matriz de adjacencia
int dimension; // quantidade total de vertices

void printData(){
	int i, j;
	cout << "Dimension: " << dimension << endl;
	for(i = 1; i <= dimension; i++){
		for(j = 1; j <= dimension; j++){
			cout << matrizAdj[i][j] << " ";
		}
		cout << endl;
	}


}

int Swap(vector<int> &solucao, int custoSolucaoAnterior){
	int i, j;
	int custoVertRetirados = 0, custoVertInseridos = 0, custoDaSolucao = custoSolucaoAnterior;
	int tamanhoDaSolucao = solucao.size();
	int diferencaDeCusto = 0;
	tSwap melhoresVertices;

	while(1){
		for(i = 1; i < tamanhoDaSolucao - 3; i++){
			for(j = i + 2; j< tamanhoDaSolucao - 1; j++){
				custoVertRetirados = matrizAdj[solucao[i-1]][solucao[i]] + matrizAdj[solucao[j-1]][solucao[j]] +
														 matrizAdj[solucao[i+1]][solucao[i]] + matrizAdj[solucao[j+1]][solucao[j]];
				custoVertInseridos = matrizAdj[solucao[i-1]][solucao[j]] + matrizAdj[solucao[j-1]][solucao[i]] +
														 matrizAdj[solucao[i+1]][solucao[j]] + matrizAdj[solucao[j+1]][solucao[i]];
				
				if((custoVertInseridos - custoVertRetirados) < diferencaDeCusto){
					diferencaDeCusto = custoVertInseridos - custoVertRetirados;

					melhoresVertices.posicaoVertice1 = i;
					melhoresVertices.posicaoVertice2 = j;
					melhoresVertices.vertice1 = solucao[i];
					melhoresVertices.vertice2 = solucao[j];
				}
			}
		}

		if(diferencaDeCusto < 0){
			custoDaSolucao = custoDaSolucao + diferencaDeCusto;

			solucao.erase(solucao.begin()+ melhoresVertices.posicaoVertice1);
			solucao.emplace(solucao.begin()+ melhoresVertices.posicaoVertice1, melhoresVertices.vertice1);

			solucao.erase(solucao.begin()+ melhoresVertices.posicaoVertice2);
			solucao.emplace(solucao.begin()+ melhoresVertices.posicaoVertice2, melhoresVertices.vertice2);

			diferencaDeCusto = 0;
		}else{
			break;
		}
	}

	return custoDaSolucao;
}

int Reinsercao(vector<int> &solucao, int tamanhoDoBloco, int custoSolucaoAnterior){
	int i, j;
	int custoVertRetirados = 0, custoVertInseridos = 0, custoDaSolucao = custoSolucaoAnterior;
	int tamanhoDaSolucao = solucao.size();
	int diferencaDeCusto = 0;
	bool flag = false;
	tReinsercao melhorReinsercao;

	cout << tamanhoDaSolucao << endl;

	while(1){
		for(i = 1; i < tamanhoDaSolucao - 1 - tamanhoDoBloco; i++){
			custoVertRetirados = (matrizAdj[solucao[i-1]][solucao[i + tamanhoDoBloco]]) - 
													 (matrizAdj[solucao[i]][solucao[i-1]] + matrizAdj[solucao[i+tamanhoDoBloco]][i + tamanhoDoBloco - 1]);
			
			for(j = i + tamanhoDoBloco; j < tamanhoDaSolucao - tamanhoDoBloco; j++){
				custoVertInseridos = (matrizAdj[solucao[i]][solucao[j]] + matrizAdj[solucao[j+1]][solucao[i + tamanhoDoBloco-1]]) - 
														 (matrizAdj[solucao[j]][solucao[j+1]]);								 
				
				if((custoVertInseridos + custoVertRetirados) < diferencaDeCusto){
					flag = true;
					diferencaDeCusto = custoVertInseridos + custoVertRetirados;
					
					cout << "verices", i, j;

					cout << "psiu " << diferencaDeCusto << " " << i <<" " << j<< endl;

					melhorReinsercao.posicaoDoVertice = i;
					melhorReinsercao.vertice = solucao[i];
					melhorReinsercao.posicaoDeInsercao = j+1;
				}
			}
		}

		//cout << "sai" << endl;
		if(flag){
			//cout << "sai" << endl;
			flag = false;
			custoDaSolucao += diferencaDeCusto;
			diferencaDeCusto = 0;

			if(tamanhoDoBloco == 1){
				solucao.insert(solucao.begin() + melhorReinsercao.posicaoDeInsercao, solucao[melhorReinsercao.posicaoDoVertice]);
				solucao.erase(solucao.begin() + melhorReinsercao.posicaoDoVertice);
			}else{
				solucao.insert(solucao.begin() + melhorReinsercao.posicaoDeInsercao, solucao.begin() + melhorReinsercao.posicaoDoVertice, solucao.begin() + melhorReinsercao.posicaoDoVertice + tamanhoDoBloco);
				solucao.erase(solucao.begin() + melhorReinsercao.posicaoDoVertice, solucao.begin() + melhorReinsercao.posicaoDoVertice + tamanhoDoBloco);
			}
		}else{
			break;
		}
	}

	return custoDaSolucao;
}

bool comparaCusto(const tConstrutivo &dado1, const tConstrutivo &dado2){
  return dado1.custo < dado2.custo;
}

int Construtivo(vector<int> &solucao, int inicio, float alpha){
	int i, j;
	int verticeEscolhido = 0, custoDaSolucao = 0, tamanhoListaDeVertices = 0;
	int quantVertices = 0, tamanhoDaSolucao = 0;

	vector<int> listaDeVertices;
	vector<tConstrutivo> listaMelhorVertices;
	tConstrutivo lista;

	solucao.push_back(inicio);

	for(i = 1; i <= dimension; i++){
		if(i == inicio){
			continue;
		} 

		listaDeVertices.push_back(i);
	}

	verticeEscolhido = rand() % listaDeVertices.size()+1;

	solucao.push_back(listaDeVertices[verticeEscolhido]);

	listaDeVertices.erase(listaDeVertices.begin() + verticeEscolhido);

	custoDaSolucao += matrizAdj[solucao[0]][solucao[1]];

	while(1){
		quantVertices = listaDeVertices.size();
		tamanhoDaSolucao = solucao.size();

		//cout << "tamanho" << quantVertices << " "<< tamanhoDaSolucao << " "<< solucao[0] << endl;
		

		 
		for (i = 0; i < quantVertices; i++) { //custo de adiconar os vertices restante na solução
      for (j = 1; j < tamanhoDaSolucao; j++) {
        lista.vertice = listaDeVertices[i];
        lista.posicao = j;
        lista.custo = (matrizAdj[solucao[j-1]][listaDeVertices[i]] + matrizAdj[listaDeVertices[i]][solucao[j]]) - matrizAdj[solucao[j-1]][solucao[j]];
        listaMelhorVertices.push_back(lista);
        //cout<< "adicionei" << endl;
      }
    }

    sort(listaMelhorVertices.begin(), listaMelhorVertices.end(), comparaCusto);

    //cout << "cheguei" << listaMelhorVertices.size()  << "agora"<< quantVertices << " " <<tamanhoDaSolucao << endl;

    verticeEscolhido = (int)(rand() % listaMelhorVertices.size()+1) * alpha;

    //cout << "cheguei" << verticeEscolhido << endl;

    solucao.emplace(solucao.begin() + listaMelhorVertices[verticeEscolhido].posicao, listaMelhorVertices[verticeEscolhido].vertice);
		
		//cout << "timm" << solucao.size() << endl;

		custoDaSolucao += listaMelhorVertices[verticeEscolhido].custo;

		for(i = 0; i < quantVertices; i++){
			if(listaMelhorVertices[verticeEscolhido].vertice == listaDeVertices[i]){
				listaDeVertices.erase(listaDeVertices.begin() + i);
				break;
			}
		}

		listaMelhorVertices.clear();

		//cout << "terminei" << listaDeVertices.size()<< " " << quantVertices << endl;


		if(quantVertices == 1){
			break;
		}
	}

	solucao.push_back(inicio);
	custoDaSolucao += matrizAdj[solucao[solucao.size() - 2]][inicio];
	//cout <<"aqq" << solucao[solucao.size() - 2] << endl;
		for(i = 0; i < solucao.size(); i++){
			cout << solucao[i] << " ";
		}

		cout << endl << custoDaSolucao << endl;

		return custoDaSolucao;
}

int RVND(vector<int> &solucao, int custoDaSolucao){
	int i;
	int custoAtual;
	vector<int> solucaoParcial;
	vector<int> vizinhancaAtual = {1, 2};
	vector<int> vizinhanca = {1, 2};
	int tamanhoDaSolucao = solucao.size(), tamanhoVizinhanca = vizinhanca.size(), vizinhancaEscolhida = 0;

	for(i = 0; i < tamanhoDaSolucao; i++){
		solucaoParcial.push_back(solucao[i]);
	}

	while(1){
		vizinhancaEscolhida = (int)(rand() % (vizinhancaAtual.size()));

		switch(vizinhancaAtual[vizinhancaEscolhida]){
			case 1:
				custoAtual = Reinsercao(solucaoParcial, 1, custoDaSolucao);

			case 2:
				custoAtual = Swap(solucaoParcial, custoDaSolucao);

			default:
				break;
		}

		if(custoAtual < custoDaSolucao){
			custoDaSolucao = custoAtual;
		
			solucao.clear();
			for(i = 0; i < tamanhoDaSolucao; i++){
				solucao.push_back(solucaoParcial[i]);
			}

			vizinhancaAtual.clear();
			for(i = 0; i < tamanhoVizinhanca; i++){
				vizinhancaAtual.push_back(vizinhanca[i]);
			}
		}else{
			vizinhancaAtual.erase(vizinhanca.begin() + vizinhancaEscolhida);
		}

		if(vizinhancaAtual.size() == 0){
				break;
			}
	}

	return custoDaSolucao;
}


//MAIN
int main(int argc, char** argv){

  readData(argc, argv, &dimension, &matrizAdj);
  //printData();

  vector<int> solucao;
  int custo = 0, nu = 1;
  float alpha = 0.8;
  
  //RVND(solucao, num);

  custo = Construtivo(solucao, 1, alpha);

  //Reinsercao(solucao, 1, custo);

 	Swap(solucao, custo);

  return 0;
}
