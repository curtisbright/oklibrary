# Oleg Zaikin, 9.8.2021 (Swansea)
# Copyright 2021 Oliver Kullmann
# This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
# it and/or modify it under the terms of the GNU General Public License as published by
# the Free Software Foundation and included in this library; either version 3 of the
# License, or any later version.

# Calculate probabilities of an orthogonal mate existance for a random Euler square
# completion problem.

# Usage:
# CalculateEulerProbabilities.R solver_results

# Example:
# CalculateEulerProbabilities.R Results_N5

version = "0.0.6"

args = commandArgs(trailingOnly = TRUE)
cat("CalculateProbabilities, version=", version, "\n", sep="")

if(length(args) > 0) {
    cat("Command line parameters :\n")
    print(args)
}

if(length(args) < 1) {
  cat("Usage: script solver_results\n")
  quit("yes")
}

solver_results = args[1]

E = read.table(solver_results, header=TRUE, stringsAsFactors=FALSE, sep=" ")

cat("total rows :", nrow(E), "\n\n", sep=" ")

N = min(E$N)
cat("N :", N, "\n", sep=" ")

minm = min(E$m)
maxm = max(E$m)
cat("minimal m :", minm, "\n", sep=" ")
cat("maximal m :", maxm, "\n", sep=" ")
cat("\n")

probabilites = vector()
mvalues = vector()

for(m in minm:maxm) {
    mvalues = append(mvalues, m)
    Em = E[E$m == m,]
    solver_runs = nrow(Em)
    if (solver_runs == 0) {
        cat("0 solver runs for m ", m, "\n", sep="")
        quit("yes")
    }
    Em_sat = Em[Em$sat == 1,]
    solutions = nrow(Em_sat)
    addit_symbol = ""
    if (m < 10) {
        addit_symbol = "0";
    }
    prob = solutions / solver_runs
    probabilites = append(probabilites, prob)
    cat("m : ", addit_symbol, m, "  solver_runs : ", solver_runs, "  solutions: ", solutions, "  prob : ", prob, "\n", sep="")
}

cat("\n")
cat("m prob", "\n", sep="")
for(i in 1:length(mvalues)) {
    cat(mvalues[i], " ", probabilites[i], "\n", sep="")
}
cat("\n")

minprob = min(probabilites)
stepm = ceiling((maxm-minm+1)/25)

library(ggplot2)
title = paste("N=", N, sep="")
qplot(x=mvalues, y=probabilites, xlab="first Latin square's filled cells", ylab="probability", main=title) +
scale_x_continuous(limits=c(minm-1, maxm+1), breaks=seq(minm, maxm, stepm), expand = c(0, 0)) +
scale_y_continuous(limits=c(0, 1.05), breaks=seq(0, 1, 0.05), expand = c(0, 0), position = "right") +
theme_linedraw() +
theme(panel.grid.minor.x = element_blank(), panel.grid.minor.y = element_blank())

ggsave(paste("plot_N=", N, ".eps", sep=""), device="eps", width=8, height=8)

# check whether the unwanted file exists and remove it
file.exists("Rplots.pdf")
file.remove("Rplots.pdf")
