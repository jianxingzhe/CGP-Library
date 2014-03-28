/*
	This file is part of CGP-Library
	Copyright (c) Andrew James Turner 2014 (andrew.turner@york.ac.uk)

    CGP-Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published 
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CGP-Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with CGP-Library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <math.h>
#include "../include/cgp.h"  

#define POPULATIONSIZE 5
#define NUMINPUTS 1
#define NUMNODES 30
#define NUMOUTPUTS 1
#define ARITY 2

int main(void){
	
	int i, gen;
	
	struct parameters *params = NULL;
	struct chromosome *population[POPULATIONSIZE];
	struct chromosome *fittestChromosome = NULL;
	struct dataSet *trainingData = NULL;
		
	float targetFitness = 1;
	int maxGens = 10000;
		
	float testInputs[NUMINPUTS];
	float testOutputs[NUMOUTPUTS];
	
	params = initialiseParameters(NUMINPUTS, NUMNODES, NUMOUTPUTS, ARITY);
	
	addNodeFunction(params, "add,sub,mul,sq,cube,sin");
	setTargetFitness(params, targetFitness);
	setMutationType(params, "point");
	setMutationRate(params, 0.08);
	
	trainingData = initialiseDataSetFromFile("./examples/symbolic.data");
	
	for(i=0; i<POPULATIONSIZE; i++){
		population[i] = initialiseChromosome(params);
	}
	
	fittestChromosome = initialiseChromosome(params);
	
	// for the number of allowed generations
	for(gen=0; gen<maxGens; gen++){
		
		// set the fitnesses of the population of chromosomes
		for(i=0; i<POPULATIONSIZE; i++){
			setChromosomeFitness(params, population[i], trainingData);
		}
		
		// copy over the last chromosome to fittestChromosome
		copyChromosome(fittestChromosome, population[POPULATIONSIZE - 1]);
		
		// for all chromosomes except the last
		for(i=0; i<POPULATIONSIZE-1; i++){
			
			// copy ith chromosome to fittestChromosome if fitter
			if(getChromosomeFitness(population[i]) < getChromosomeFitness(fittestChromosome)){
				copyChromosome(fittestChromosome, population[i]);
			}
		}
		
		// termination condition
		if(getChromosomeFitness(fittestChromosome) <= targetFitness){
			break;
		}
				
		// set the first member of the population to be the fittest chromosome
		copyChromosome(population[0], fittestChromosome);
		
		// set remaining member of the population to be mutations of the
		// fittest chromosome
		for(i=1; i<POPULATIONSIZE; i++){
			
			copyChromosome(population[i], fittestChromosome);
			mutateChromosome(params, population[i]);
		}
	}
	
	
	printf("Fittest chromosome found has fitness: %f (target %.2f) and %d active nodes.\n", getChromosomeFitness(fittestChromosome), targetFitness, getNumChromosomeActiveNodes(fittestChromosome));
	
	printf("\nFittest chromosome");
	printChromosome(fittestChromosome);
	
	removeInactiveNodes(fittestChromosome);
		
	saveChromosome(fittestChromosome, "fittestChromosome.chromo");
	
	freeChromosome(fittestChromosome);
	
	fittestChromosome = initialiseChromosomeFromFile("fittestChromosome.chromo");
	
	printf("\nFittest chromosome without inactive nodes (loaded from file).\n");
	printChromosome(fittestChromosome);
		
	testInputs[0] = 3;
	
	executeChromosome(fittestChromosome, testInputs, testOutputs);
	printf("Using the generated chromosome\n");
	printf("Applied input: %f\n", testInputs[0]);
	printf("Generated output: %f\n", testOutputs[0]);
	
	
	for(i=0; i<POPULATIONSIZE; i++){
		freeChromosome(population[i]);
	}
	
	freeChromosome(fittestChromosome);
	freeDataSet(trainingData);
	freeParameters(params);
	
	return 1;
}