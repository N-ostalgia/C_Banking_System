set terminal png size 800,600
set output 'transactions_mensuelles.png'
set title 'Transactions Mensuelles'
set xlabel 'Mois'
set ylabel 'Nombre de Transactions'
set xtics 1
set grid
plot 'transactions_mensuelles.txt' using 1:2 with linespoints title 'Transactions'
set output 'types_comptes.png'
set title 'Repartition des Types de Comptes'
set xlabel 'Type de Compte'
set ylabel 'Nombre de Comptes'
set style data histogram
set style fill solid
set boxwidth 0.5
plot 'types_comptes.txt' using 2:xtic(1) title 'Types'
