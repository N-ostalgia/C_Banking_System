set terminal png size 800,600
set output 'transactions_mensuelles.png'
set title 'Transactions Mensuelles'
set xlabel 'Mois'
set ylabel 'Montant (DH)'
plot 'transactions_mensuelles.txt' using 1:2 with lines title 'Transactions'
