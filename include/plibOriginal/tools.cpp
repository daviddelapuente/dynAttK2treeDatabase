#include <stdio.h>
#include <math.h>
#include <string.h>
#include <malloc.h>

//Obtener el numero de nodos
int numNodes(char * basename){
	int freq;
	char nameFile[256];
	sprintf(nameFile,"%s.nodes.typ",basename);
	FILE * fNodesTyp = fopen(nameFile,"r");
	int totalNodes=0;
	while(fscanf(fNodesTyp,"%*s\t%d\n",&freq)==1){
		totalNodes += freq;
	}
	fclose(fNodesTyp);
	return totalNodes;
}

//Obtener el numero de aristas
int numEdges(char * basename){
	int freq;
	char nameFile[256];
	sprintf(nameFile,"%s.edges.typ",basename);
	FILE * fEdgesTyp = fopen(nameFile,"r");
	if (fEdgesTyp != NULL) {
		int totalEdges = 0;
		while (fscanf(fEdgesTyp, "%*s\t%d\n", &freq) == 1) {
			totalEdges += freq;
		}
		fclose(fEdgesTyp);
		return totalEdges;
	}
	return 0;
}

//Obtener tamaño de submatriz (minimo mas proximo al numero de nodos)
int getSizeMat(int nodes){
	int tamSubm = 1<<26;
	while (tamSubm>2*nodes){
		tamSubm/=2;
	}
	return tamSubm;
}

//Obtener submatriz pertenencia

