% An implementation of p.21 example cc


P = [1 0 1; 
	0 0 1;
	1 0 1;
	0 1 1;
	1 1 0;
	0 1 0;
	1 1 0];

H = [P eye(7,7)]

r = [1 0 0 1 0 0 1];


transpose(H)

C=r.*transpose(H)


