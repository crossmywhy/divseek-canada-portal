# DivSeek Canada Portal

The [DivSeek Canada](http://www.divseekcanada.ca)  **Portal** is a web-based platform to implement association genetics workflows supporting plant breeding and crop research focusing on large scale plant genetic resources / crop genotype-phenotype data sets whose access is brokered / managed by the project.

# Genome Canada Pilot Project

The first iteration of the platform is funded under a [Genome Canada Project](https://www.genomecanada.ca/en/divseek-canada-harnessing-genomics-accelerate-crop-improvement-canada) with co-funding from other partners.

# Documentation

Some technical notes about the portal system will be compiled on the [Divseek Portal Wiki](https://github.com/DivSeek-Canada/divseek-canada-portal/wiki).

# Working on the Project

This project resides in [this Github project repository](https://github.com/DivSeek-Canada/divseek-canada-portal).
Note that the project contains an embedded git submodule, which is code from the
[docker-tripal project v3.x branch](https://github.com/erasche/docker-tripal/tree/v3.x).

Thus, in addition to git cloning the project, i.e.

    git clone https://github.com/DivSeek-Canada/divseek-canada-portal 
    
one needs to initialize the submodule, i.e.

    cd divseek-canada-portal
    git submodule init
    
in later iterations, after every pull from the remote repo, one should update the submodule, i.e.

    git submodule update

 