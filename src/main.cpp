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

typedef struct{
	int posicaoVertice1;
	int vertice1;
	int posicaoVertice2;
	int vertice2;
}tSwap;

typedef struct{
	int posicaoDoVertice;
	int posicaoDeInsercao;
	int custor;
	int custi;
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
	bool flag = false;
	tSwap melhoresVertices;

	for(i = 1; i < tamanhoDaSolucao - 3; i++){
		for(j = i + 2; j < tamanhoDaSolucao - 1; j++){
			custoVertRetirados = matrizAdj[solucao[i-1]][solucao[i]] + matrizAdj[solucao[j-1]][solucao[j]] +
													 matrizAdj[solucao[i+1]][solucao[i]] + matrizAdj[solucao[j+1]][solucao[j]];
			custoVertInseridos = matrizAdj[solucao[i-1]][solucao[j]] + matrizAdj[solucao[j-1]][solucao[i]] +
													 matrizAdj[solucao[i+1]][solucao[j]] + matrizAdj[solucao[j+1]][solucao[i]];
			
			if((custoVertInseridos - custoVertRetirados) < diferencaDeCusto){
				flag = true;
				diferencaDeCusto = custoVertInseridos - custoVertRetirados;
				melhoresVertices.posicaoVertice1 = i;
				melhoresVertices.posicaoVertice2 = j;
				melhoresVertices.vertice1 = solucao[i];
				melhoresVertices.vertice2 = solucao[j];
			}
		}
	}

	if(flag){
		custoDaSolucao = custoDaSolucao + diferencaDeCusto;

		solucao.erase(solucao.begin()+ melhoresVertices.posicaoVertice2);
		solucao.emplace(solucao.begin()+ melhoresVertices.posicaoVertice2, melhoresVertices.vertice1);

		solucao.erase(solucao.begin()+ melhoresVertices.posicaoVertice1);
		solucao.emplace(solucao.begin()+ melhoresVertices.posicaoVertice1, melhoresVertices.vertice2);

		diferencaDeCusto = 0;
		flag = false;
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

	for(i = 1; i < tamanhoDaSolucao - 1 - tamanhoDoBloco; i++){
		custoVertRetirados = matrizAdj[solucao[i-1]][solucao[i + tamanhoDoBloco]] - (matrizAdj[solucao[i]][solucao[i-1]] + matrizAdj[solucao[i+tamanhoDoBloco]][solucao[i + tamanhoDoBloco - 1]]);
		
		for(j = i + tamanhoDoBloco; j < tamanhoDaSolucao - tamanhoDoBloco; j++){
			custoVertInseridos = (matrizAdj[solucao[i]][solucao[j]] + matrizAdj[solucao[j+1]][solucao[i + tamanhoDoBloco-1]]) - matrizAdj[solucao[j]][solucao[j+1]]; 
			
			if((custoVertInseridos + custoVertRetirados) < diferencaDeCusto){
				flag = true;
				diferencaDeCusto = custoVertInseridos + custoVertRetirados;
				
				melhorReinsercao.posicaoDoVertice = i;
				melhorReinsercao.posicaoDeInsercao = j + 1;
				melhorReinsercao.custor = custoVertRetirados;
				melhorReinsercao.custi = custoVertInseridos;
			}
		}
	}

	if(flag){
		custoDaSolucao = custoDaSolucao + diferencaDeCusto;
		

		if(tamanhoDoBloco == 1){
			solucao.insert(solucao.begin() + melhorReinsercao.posicaoDeInsercao, solucao[melhorReinsercao.posicaoDoVertice]);
			solucao.erase(solucao.begin() + melhorReinsercao.posicaoDoVertice);
		}else{
			solucao.insert(solucao.begin() + melhorReinsercao.posicaoDeInsercao, solucao.begin() + melhorReinsercao.posicaoDoVertice, solucao.begin() + melhorReinsercao.posicaoDoVertice + tamanhoDoBloco);
			solucao.erase(solucao.begin() + melhorReinsercao.posicaoDoVertice, solucao.begin() + melhorReinsercao.posicaoDoVertice + tamanhoDoBloco);
		}

		diferencaDeCusto = 0;
		flag = false;
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

	solucao.clear();

	solucao.push_back(inicio);

	for(i = 1; i <= dimension; i++){
		if(i == inicio){
			continue;
		} 

		listaDeVertices.push_back(i);
	}

	verticeEscolhido = (rand() % listaDeVertices.size()-1) + 1;

	solucao.push_back(listaDeVertices[verticeEscolhido]);

	listaDeVertices.erase(listaDeVertices.begin() + verticeEscolhido);

	custoDaSolucao += matrizAdj[solucao[0]][solucao[1]];

	while(1){
		quantVertices = listaDeVertices.size();
		tamanhoDaSolucao = solucao.size();
		 
		for (i = 0; i < quantVertices; i++) { //custo de adiconar os vertices restante na solução
	      for (j = 1; j < tamanhoDaSolucao; j++) {
	        lista.vertice = listaDeVertices[i];
	        lista.posicao = j;
	        lista.custo = (matrizAdj[solucao[j-1]][listaDeVertices[i]] + matrizAdj[listaDeVertices[i]][solucao[j]]) - matrizAdj[solucao[j-1]][solucao[j]];
	        listaMelhorVertices.push_back(lista);
	      }
    	}

	    sort(listaMelhorVertices.begin(), listaMelhorVertices.end(), comparaCusto);

	    verticeEscolhido = (int)(rand() % listaMelhorVertices.size()+1) * alpha;

	    solucao.emplace(solucao.begin() + listaMelhorVertices[verticeEscolhido].posicao, listaMelhorVertices[verticeEscolhido].vertice);

		custoDaSolucao += listaMelhorVertices[verticeEscolhido].custo;

		for(i = 0; i < quantVertices; i++){
			if(listaMelhorVertices[verticeEscolhido].vertice == listaDeVertices[i]){
				listaDeVertices.erase(listaDeVertices.begin() + i);
				break;
			}
		}

		listaMelhorVertices.clear();

		if(quantVertices == 1){
			break;
		}
	}

	solucao.push_back(inicio);
	custoDaSolucao += matrizAdj[solucao[solucao.size() - 2]][inicio];

	return custoDaSolucao;
}

int RVND(vector<int> &solucao, int custoDaSolucao){
	int i;
	int custoAtual = 0, custo = custoDaSolucao;
	vector<int> solucaoParcial;
	vector<int> vizinhanca = {1, 2};
	int tamanhoDaSolucao = solucao.size(), tamanhoVizinhanca = vizinhanca.size(), vizinhancaEscolhida = 0;

	for(i = 0; i < tamanhoDaSolucao; i++){
		solucaoParcial.push_back(solucao[i]);
	}

	while(1){
		vizinhancaEscolhida = (int)(rand() % (vizinhanca.size()));

		switch(vizinhanca[vizinhancaEscolhida]){
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

			vizinhanca.clear();
			for(i = 0; i < tamanhoVizinhanca; i++){
				vizinhanca.push_back(i);
			}
		}else{
			vizinhanca.erase(vizinhanca.begin() + vizinhancaEscolhida);
		}

		if(vizinhanca.size() == 0){
				break;
			}
	}

	return custoDaSolucao;
}

int GRASP(vector<int> &solucao, int maxIteracoes, float alpha){
	int i, custoDaSolucao = __INT_MAX__, custoAtual = 0;
	vector <int> solucaoParcial;

	for(i = 0; i < maxIteracoes; i++){
		custoAtual = Construtivo(solucaoParcial, 1, alpha);
		custoAtual = RVND(solucaoParcial, custoAtual);	

		if(custoAtual < custoDaSolucao){
			solucao = solucaoParcial;
			custoDaSolucao = custoAtual;
		}
	}

	return custoDaSolucao;
}
//MAIN
int main(int argc, char** argv){
	clock_t Ticks[2];
	double tempo = 0, tempoMedio = 0, tempoMedioEur = 0, tempoEur = 0;
	int iteracoes = 10, gap = 0, i, custoEur = 0;
	int custoMedio = 0, melhorCusto = __INT_MAX__, custoMedioEur = 0, melhorCustoEur = __INT_MAX__;

  	readData(argc, argv, &dimension, &matrizAdj);

  	ofstream saida;

  	saida.open("saida.txt", ios::app);


  	vector<int> solucao;
  	int custo = 0;
  	float alpha = 0.8;
  	
  	for(i = 0; i < iteracoes; i++){
	  	Ticks[1] = clock();
	  	custo = Construtivo(solucao, 1 , alpha);
	  	Ticks[2] = clock();

	  	tempo = ((Ticks[1] - Ticks[0]) * 1000/ CLOCKS_PER_SEC);
	  	tempoMedio+= tempo;

	  	custoMedio += custo;

	  	if(custo < melhorCusto){
	  		melhorCusto = custo;
	  	}
  	}
  	

  	tempoMedio = tempoMedio/ iteracoes;
  	custoMedio = custoMedio / iteracoes;

  	for(i = 0; i < iteracoes; i++){
	  	Ticks[1] = clock();
	  	custoEur = GRASP(solucao, 30, alpha);
	  	Ticks[2] = clock();

	  	tempoEur = ((Ticks[1] - Ticks[0]) * 1000/ CLOCKS_PER_SEC);
	  	tempoMedioEur+= tempoEur;

	  	custoMedioEur += custoEur;

	  	if(custoEur < melhorCustoEur){
	  		melhorCustoEur = custoEur;
	  	}
  	}
  	

  	tempoMedioEur = tempoMedioEur/ iteracoes;
  	custoMedioEur = custoMedioEur / iteracoes;
/*
  	saida << "alpha = " << alpha <<  ",";
  	saida << "ótimo" << ",";
  	saida << "média solução" << ",";
  	saida << "melhor solucao" << ",";
  	saida << "media tempo" << ",";
  	saida << "gap" << ",";
  	saida << "média solução" << ",";
  	saida << "melhor solucao" << ",";
  	saida << "media tempo" << ",";
  	saida << "gap" << endl;
  	*/
  	saida << argv[1] << ",";
  	saida << " " << ",";
  	saida << custoMedio << ",";
  	saida << melhorCusto << ",";
  	saida << tempoMedio << ",";
  	saida << " " << ",";
  	saida << custoMedioEur << ",";
  	saida << melhorCustoEur << ",";
  	saida << tempoMedioEur << ",";
  	saida << " " << endl;
	//custo = GRASP(solucao, 50, alpha);

 	cout << endl << "Custo final: " << custo << endl;

  	return 0;
}
