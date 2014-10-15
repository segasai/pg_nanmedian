/*
	   Copyright (C) 2014 Sergey Koposov

    Email: koposov@ast.cam.ac.uk

    This file is part of pg_nanmedian.

    pg_nanmedian is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    pg_nanmedian is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with pg_nanmedian; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include <string.h>

/* Postgres stuff */
#include "postgres.h"
#include "executor/spi.h"
#include "utils/lsyscache.h"
#include "utils/array.h"
#include "catalog/pg_type.h"
#include "fmgr.h"
#if PG_VERSION_NUM >= 90300
#include "access/tupmacs.h"
#endif


/* For PostgreSQL versions >= 8.2 */
#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif
/* End of Postgres stuff */

#include <math.h>

/* Postgres functions */
Datum pgnanmedian_f8(PG_FUNCTION_ARGS);
Datum pgnanmedian_f4(PG_FUNCTION_ARGS);
Datum pgnanmad_f8(PG_FUNCTION_ARGS);
Datum pgnanmad_f4(PG_FUNCTION_ARGS);

//Datum pgnanmad(PG_FUNCTION_ARGS);

#define MAXNEL 100

static float8 median_f8(float8 *procarr, int nc)
{
	int i, j, tmpi;
	float8 tmp;
	if (nc == 0)
	{
		return NAN;
	}
	for (i=1; i<nc; i++)
	{
		tmp = procarr[i];
		j = i;
		while( (j > 0) && (procarr[j - 1] > tmp))
		{
			procarr[j] = procarr[j-1];
			j--;
		}
		procarr[j] = tmp;
	}

	if ((nc % 2) == 0)
	{
		tmpi = nc/2;
		return ((procarr[tmpi] + procarr[tmpi -1])/2);
	}
	else
	{
		tmpi= nc/2;
		return (procarr[tmpi]);	
	}
}

static float4 median_f4(float4 *procarr, int nc)
{
	int i, j, tmpi;
	float8 tmp;
	if (nc == 0)
	{
		return NAN;
	}
	for (i=1; i<nc; i++)
	{
		tmp = procarr[i];
		j = i;
		while( (j > 0) && (procarr[j - 1] > tmp))
		{
			procarr[j] = procarr[j-1];
			j--;
		}
		procarr[j] = tmp;
	}

	if ((nc % 2) == 0)
	{
		tmpi = nc/2;
		return ((procarr[tmpi] + procarr[tmpi -1])/2);
	}
	else
	{
		tmpi= nc/2;
		return (procarr[tmpi]);	
	}
}


PG_FUNCTION_INFO_V1(pgnanmedian_f8);
Datum pgnanmedian_f8(PG_FUNCTION_ARGS)
{
	ArrayType  *arr;
	Oid elmtype;
	int16 elmlen;
	bool elmbyval;
	char elmalign;
	Datum *elemsp;
	bool *nullsp;
	int nelemsp;
	int nc, i, j;
	float8 procarr[MAXNEL],tmp;
	arr = PG_GETARG_ARRAYTYPE_P(0); 	
	//poly_nitems = ArrayGetNItems(ARR_NDIM(arr), ARR_DIMS(arr));
	elmtype = FLOAT8OID;
	get_typlenbyvalalign(elmtype, &elmlen, &elmbyval, &elmalign);
	deconstruct_array(arr, elmtype, elmlen, elmbyval, elmalign,
			 &elemsp, &nullsp, &nelemsp);
	if (nelemsp > MAXNEL)
	{
		elog(ERROR, "Too many elements -- dont support");
	}
	for (i=0, j=0; i < nelemsp; i++)
	{
		if (nullsp[i]) { continue; }
		tmp = DatumGetFloat8(elemsp[i]);
		if isnan(tmp) { continue;}
		procarr[j++] = tmp;
	}
	nc = j;

	pfree(elemsp);
	pfree(nullsp);

	PG_RETURN_FLOAT8(median_f8(procarr, nc));
}



PG_FUNCTION_INFO_V1(pgnanmedian_f4);
Datum pgnanmedian_f4(PG_FUNCTION_ARGS)
{
	ArrayType  *arr;
	Oid elmtype;
	int16 elmlen;
	bool elmbyval;
	char elmalign;
	Datum *elemsp;
	bool *nullsp;
	int nelemsp;
	int nc, i, j;
	float4 procarr[MAXNEL],tmp;
	arr = PG_GETARG_ARRAYTYPE_P(0); 	
	//poly_nitems = ArrayGetNItems(ARR_NDIM(arr), ARR_DIMS(arr));
	elmtype = FLOAT4OID;
	get_typlenbyvalalign(elmtype, &elmlen, &elmbyval, &elmalign);
	deconstruct_array(arr, elmtype, elmlen, elmbyval, elmalign,
			 &elemsp, &nullsp, &nelemsp);
	if (nelemsp > MAXNEL)
	{
		elog(ERROR, "Too many elements -- dont support");
	}
	for (i=0, j=0; i < nelemsp; i++)
	{
		if (nullsp[i]) { continue; }
		tmp = DatumGetFloat4(elemsp[i]);
		if isnan(tmp) { continue;}
		procarr[j++] = tmp;
	}
	nc = j;

	pfree(elemsp);
	pfree(nullsp);

	PG_RETURN_FLOAT4(median_f4(procarr, nc));
}


PG_FUNCTION_INFO_V1(pgnanmad_f8);
Datum pgnanmad_f8(PG_FUNCTION_ARGS)
{
	ArrayType  *arr;
	Oid elmtype;
	int16 elmlen;
	bool elmbyval;
	char elmalign;
	Datum *elemsp;
	bool *nullsp;
	int nelemsp;
	int nc, i, j;
	float8 procarr[MAXNEL],tmp;
	float8 med;
	arr = PG_GETARG_ARRAYTYPE_P(0); 	
	//poly_nitems = ArrayGetNItems(ARR_NDIM(arr), ARR_DIMS(arr));
	elmtype = FLOAT8OID;
	get_typlenbyvalalign(elmtype, &elmlen, &elmbyval, &elmalign);
	deconstruct_array(arr, elmtype, elmlen, elmbyval, elmalign,
			 &elemsp, &nullsp, &nelemsp);
	if (nelemsp > MAXNEL)
	{
		elog(ERROR, "Too many elements -- dont support");
	}
	for (i=0, j=0; i < nelemsp; i++)
	{
		if (nullsp[i]) { continue; }
		tmp = DatumGetFloat8(elemsp[i]);
		if isnan(tmp) { continue;}
		procarr[j++] = tmp;
	}
	nc = j;

	pfree(elemsp);
	pfree(nullsp);

	med = median_f8(procarr, nc);
	for (i=0;i<nc;i++)
	{
	    procarr[i] = fabs(procarr[i]-med);
	}
	PG_RETURN_FLOAT8(median_f8(procarr, med));
}



PG_FUNCTION_INFO_V1(pgnanmad_f4);
Datum pgnanmad_f4(PG_FUNCTION_ARGS)
{
	ArrayType  *arr;
	Oid elmtype;
	int16 elmlen;
	bool elmbyval;
	char elmalign;
	Datum *elemsp;
	bool *nullsp;
	int nelemsp;
	int nc, i, j;
	float4 procarr[MAXNEL], med,tmp;
	arr = PG_GETARG_ARRAYTYPE_P(0); 	
	//poly_nitems = ArrayGetNItems(ARR_NDIM(arr), ARR_DIMS(arr));
	elmtype = FLOAT4OID;
	get_typlenbyvalalign(elmtype, &elmlen, &elmbyval, &elmalign);
	deconstruct_array(arr, elmtype, elmlen, elmbyval, elmalign,
			 &elemsp, &nullsp, &nelemsp);
	if (nelemsp > MAXNEL)
	{
		elog(ERROR, "Too many elements -- dont support");
	}
	for (i=0, j=0; i < nelemsp; i++)
	{
		if (nullsp[i]) { continue; }
		tmp = DatumGetFloat4(elemsp[i]);
		if isnan(tmp) { continue;}
		procarr[j++] = tmp;
	}
	nc = j;

	pfree(elemsp);
	pfree(nullsp);

	med = median_f4(procarr, nc);
	for (i=0;i<nc;i++)
	{
	    procarr[i]=fabs(procarr[i]-med);
	}
	PG_RETURN_FLOAT4(median_f4(procarr, med));

}



