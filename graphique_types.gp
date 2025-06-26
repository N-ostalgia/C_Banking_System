set terminal png size 800,600
set output 'types_comptes.png'
set title 'Distribution des Types de Comptes'
set xlabel 'Type de Compte'
set ylabel 'Nombre de Comptes'
plot 'types_comptes.txt' using 1:2 with boxes title 'Comptes'
