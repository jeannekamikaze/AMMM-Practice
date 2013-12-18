int size=...;

float lambda = 1;

range rows=1..size;
range cols=1..size;

range rows_minus_one=1..(size-1);
range cols_minus_one=1..(size-1);

float D[r in rows, c in cols]=...;

float RIGHT[r in rows, c in cols_minus_one]=...;
float DOWN[r in rows_minus_one, c in cols]=...;

dvar boolean pixel_class[r in rows, c in cols];
dvar boolean right_abs[r in rows, c in cols_minus_one];
dvar boolean down_abs[r in rows_minus_one, c in cols];

minimize sum(r in rows) sum(c in cols) D[r,c]*pixel_class[r,c] 
         + lambda * sum(r in rows) sum(c in cols_minus_one) RIGHT[r,c]*right_abs[r,c]
         + lambda * sum(r in rows_minus_one) sum(c in cols) DOWN[r,c]*down_abs[r,c];
         
subject to {
	forall(r in rows, c in cols_minus_one)
		right_abs[r,c] >= pixel_class[r,c] - pixel_class[r,c+1];

	forall(r in rows, c in cols_minus_one)
		right_abs[r,c] >= pixel_class[r,c+1] - pixel_class[r,c];
	
	forall(r in rows_minus_one, c in cols)
		down_abs[r,c] >= pixel_class[r,c] - pixel_class[r+1,c];

	forall(r in rows_minus_one, c in cols)
		down_abs[r,c] >= pixel_class[r+1,c] - pixel_class[r,c];
}

execute {
	for (var r=1; r<=size; ++r) {
		for (var c=1; c<=size; ++c) {
			write(pixel_class[r][c]);
			if (c != size) write("\t");
 		}			
		writeln("");
 	}			
 }
 