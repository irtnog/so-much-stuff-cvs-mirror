/* xllist.c - xlisp built-in list functions */
/*	Copyright (c) 1985, by David Michael Betz
	All Rights Reserved
	Permission is granted for unrestricted non-commercial use	*/

#include "xlisp.h"

/* forward declarations */
LOCAL   LVAL cxr();
LOCAL   LVAL nth(),assoc(),remif(),delif(),delif(),sortlist(),gluelists();
LOCAL   LVAL subst(),sublis(),map();
LOCAL   splitlist();

/* xcar - take the car of a cons cell */
LVAL xcar()
{
    LVAL list;
    list = xlgalist();
    xllastarg();
    return (list ? car(list) : NIL);
}

/* xcdr - take the cdr of a cons cell */
LVAL xcdr()
{
    LVAL list;
    list = xlgalist();
    xllastarg();
    return (list ? cdr(list) : NIL);
}

/* cxxr functions */
LVAL xcaar() { return (cxr("aa")); }
LVAL xcadr() { return (cxr("da")); }
LVAL xcdar() { return (cxr("ad")); }
LVAL xcddr() { return (cxr("dd")); }

/* cxxxr functions */
LVAL xcaaar() { return (cxr("aaa")); }
LVAL xcaadr() { return (cxr("daa")); }
LVAL xcadar() { return (cxr("ada")); }
LVAL xcaddr() { return (cxr("dda")); }
LVAL xcdaar() { return (cxr("aad")); }
LVAL xcdadr() { return (cxr("dad")); }
LVAL xcddar() { return (cxr("add")); }
LVAL xcdddr() { return (cxr("ddd")); }

/* cxxxxr functions */
LVAL xcaaaar() { return (cxr("aaaa")); }
LVAL xcaaadr() { return (cxr("daaa")); }
LVAL xcaadar() { return (cxr("adaa")); }
LVAL xcaaddr() { return (cxr("ddaa")); }
LVAL xcadaar() { return (cxr("aada")); }
LVAL xcadadr() { return (cxr("dada")); }
LVAL xcaddar() { return (cxr("adda")); }
LVAL xcadddr() { return (cxr("ddda")); }
LVAL xcdaaar() { return (cxr("aaad")); }
LVAL xcdaadr() { return (cxr("daad")); }
LVAL xcdadar() { return (cxr("adad")); }
LVAL xcdaddr() { return (cxr("ddad")); }
LVAL xcddaar() { return (cxr("aadd")); }
LVAL xcddadr() { return (cxr("dadd")); }
LVAL xcdddar() { return (cxr("addd")); }
LVAL xcddddr() { return (cxr("dddd")); }

/* cxr - common car/cdr routine */
LOCAL LVAL cxr(adstr)
  char *adstr;
{
    LVAL list;

    /* get the list */
    list = xlgalist();
    xllastarg();

    /* perform the car/cdr operations */
    while (*adstr && consp(list))
	list = (*adstr++ == 'a' ? car(list) : cdr(list));

    /* make sure the operation succeeded */
    if (*adstr && list)
	xlfail("bad argument");

    /* return the result */
    return (list);
}

/* xcons - construct a new list cell */
LVAL xcons()
{
    LVAL arg1,arg2;

    /* get the two arguments */
    arg1 = xlgetarg();
    arg2 = xlgetarg();
    xllastarg();

    /* construct a new list element */
    return (cons(arg1,arg2));
}

/* xlist - built a list of the arguments */
LVAL xlist()
{
    LVAL last,next,val;

    /* protect some pointers */
    xlsave1(val);

    /* add each argument to the list */
    for (val = NIL; moreargs(); ) {

	/* append this argument to the end of the list */
	next = consa(nextarg());
	if (val) rplacd(last,next);
	else val = next;
	last = next;
    }

    /* restore the stack */
    xlpop();

    /* return the list */
    return (val);
}

/* xappend - built-in function append */
LVAL xappend()
{
    LVAL list,last,next,val;

    /* protect some pointers */
    xlsave1(val);

    /* initialize */
    val = NIL;
    
    /* append each argument */
    if (moreargs()) {
	while (xlargc > 1) {

	    /* append each element of this list to the result list */
	    for (list = nextarg(); consp(list); list = cdr(list)) {
		next = consa(car(list));
		if (val) rplacd(last,next);
		else val = next;
		last = next;
	    }
	}

	/* handle the last argument */
	if (val) rplacd(last,nextarg());
	else val = nextarg();
    }

    /* restore the stack */
    xlpop();

    /* return the list */
    return (val);
}

/* xreverse - built-in function reverse */
LVAL xreverse()
{
    LVAL list,val;

    /* protect some pointers */
    xlsave1(val);

    /* get the list to reverse */
    list = xlgalist();
    xllastarg();

    /* append each element to the head of the result list */
    for (val = NIL; consp(list); list = cdr(list))
	val = cons(car(list),val);

    /* restore the stack */
    xlpop();

    /* return the list */
    return (val);
}

/* xlast - return the last cons of a list */
LVAL xlast()
{
    LVAL list;

    /* get the list */
    list = xlgalist();
    xllastarg();

    /* find the last cons */
    while (consp(list) && cdr(list))
	list = cdr(list);

    /* return the last element */
    return (list);
}

/* xmember - built-in function 'member' */
LVAL xmember()
{
    LVAL x,list,fcn,val;
    int tresult;

    /* protect some pointers */
    xlsave1(fcn);

    /* get the expression to look for and the list */
    x = xlgetarg();
    list = xlgalist();
    xltest(&fcn,&tresult);

    /* look for the expression */
    for (val = NIL; consp(list); list = cdr(list))
	if (dotest2(x,car(list),fcn) == tresult) {
	    val = list;
	    break;
	}

    /* restore the stack */
    xlpop();

    /* return the result */
    return (val);
}

/* xassoc - built-in function 'assoc' */
LVAL xassoc()
{
    LVAL x,alist,fcn,pair,val;
    int tresult;

    /* protect some pointers */
    xlsave1(fcn);

    /* get the expression to look for and the association list */
    x = xlgetarg();
    alist = xlgalist();
    xltest(&fcn,&tresult);

    /* look for the expression */
    for (val = NIL; consp(alist); alist = cdr(alist))
	if ((pair = car(alist)) && consp(pair))
	    if (dotest2(x,car(pair),fcn) == tresult) {
		val = pair;
		break;
	    }

    /* restore the stack */
    xlpop();

    /* return result */
    return (val);
}

/* xsubst - substitute one expression for another */
LVAL xsubst()
{
    LVAL to,from,expr,fcn,val;
    int tresult;

    /* protect some pointers */
    xlsave1(fcn);

    /* get the to value, the from value and the expression */
    to = xlgetarg();
    from = xlgetarg();
    expr = xlgetarg();
    xltest(&fcn,&tresult);

    /* do the substitution */
    val = subst(to,from,expr,fcn,tresult);

    /* restore the stack */
    xlpop();

    /* return the result */
    return (val);
}

/* subst - substitute one expression for another */
LOCAL LVAL subst(to,from,expr,fcn,tresult)
  LVAL to,from,expr,fcn; int tresult;
{
    LVAL carval,cdrval;

    if (dotest2(expr,from,fcn) == tresult)
	return (to);
    else if (consp(expr)) {
	xlsave1(carval);
	carval = subst(to,from,car(expr),fcn,tresult);
	cdrval = subst(to,from,cdr(expr),fcn,tresult);
	xlpop();
	return (cons(carval,cdrval));
    }
    else
	return (expr);
}

/* xsublis - substitute using an association list */
LVAL xsublis()
{
    LVAL alist,expr,fcn,val;
    int tresult;

    /* protect some pointers */
    xlsave1(fcn);

    /* get the assocation list and the expression */
    alist = xlgalist();
    expr = xlgetarg();
    xltest(&fcn,&tresult);

    /* do the substitution */
    val = sublis(alist,expr,fcn,tresult);

    /* restore the stack */
    xlpop();

    /* return the result */
    return (val);
}

/* sublis - substitute using an association list */
LOCAL LVAL sublis(alist,expr,fcn,tresult)
  LVAL alist,expr,fcn; int tresult;
{
    LVAL carval,cdrval,pair;

    if (pair = assoc(expr,alist,fcn,tresult))
	return (cdr(pair));
    else if (consp(expr)) {
	xlsave1(carval);
	carval = sublis(alist,car(expr),fcn,tresult);
	cdrval = sublis(alist,cdr(expr),fcn,tresult);
	xlpop();
	return (cons(carval,cdrval));
    }
    else
	return (expr);
}

/* assoc - find a pair in an association list */
LOCAL LVAL assoc(expr,alist,fcn,tresult)
  LVAL expr,alist,fcn; int tresult;
{
    LVAL pair;

    for (; consp(alist); alist = cdr(alist))
	if ((pair = car(alist)) && consp(pair))
	    if (dotest2(expr,car(pair),fcn) == tresult)
		return (pair);
    return (NIL);
}

/* xremove - built-in function 'remove' */
LVAL xremove()
{
    LVAL x,list,fcn,val,last,next;
    int tresult;

    /* protect some pointers */
    xlstkcheck(2);
    xlsave(fcn);
    xlsave(val);

    /* get the expression to remove and the list */
    x = xlgetarg();
    list = xlgalist();
    xltest(&fcn,&tresult);

    /* remove matches */
    for (; consp(list); list = cdr(list))

	/* check to see if this element should be deleted */
	if (dotest2(x,car(list),fcn) != tresult) {
	    next = consa(car(list));
	    if (val) rplacd(last,next);
	    else val = next;
	    last = next;
	}

    /* restore the stack */
    xlpopn(2);

    /* return the updated list */
    return (val);
}

/* xremif - built-in function 'remove-if' */
LVAL xremif()
{
    LVAL remif();
    return (remif(TRUE));
}

/* xremifnot - built-in function 'remove-if-not' */
LVAL xremifnot()
{
    LVAL remif();
    return (remif(FALSE));
}

/* remif - common code for 'remove-if' and 'remove-if-not' */
LOCAL LVAL remif(tresult)
  int tresult;
{
    LVAL list,fcn,val,last,next;

    /* protect some pointers */
    xlstkcheck(2);
    xlsave(fcn);
    xlsave(val);

    /* get the expression to remove and the list */
    fcn = xlgetarg();
    list = xlgalist();
    xllastarg();

    /* remove matches */
    for (; consp(list); list = cdr(list))

	/* check to see if this element should be deleted */
	if (dotest1(car(list),fcn) != tresult) {
	    next = consa(car(list));
	    if (val) rplacd(last,next);
	    else val = next;
	    last = next;
	}

    /* restore the stack */
    xlpopn(2);

    /* return the updated list */
    return (val);
}

/* dotest1 - call a test function with one argument */
int dotest1(arg,fun)
  LVAL arg,fun;
{
    LVAL *newfp;

    /* create the new call frame */
    newfp = xlsp;
    pusharg(cvfixnum((FIXTYPE)(newfp - xlfp)));
    pusharg(fun);
    pusharg(cvfixnum((FIXTYPE)1));
    pusharg(arg);
    xlfp = newfp;

    /* return the result of applying the test function */
    return (xlapply(1) != NIL);

}

/* dotest2 - call a test function with two arguments */
int dotest2(arg1,arg2,fun)
  LVAL arg1,arg2,fun;
{
    LVAL *newfp;

    /* create the new call frame */
    newfp = xlsp;
    pusharg(cvfixnum((FIXTYPE)(newfp - xlfp)));
    pusharg(fun);
    pusharg(cvfixnum((FIXTYPE)2));
    pusharg(arg1);
    pusharg(arg2);
    xlfp = newfp;

    /* return the result of applying the test function */
    return (xlapply(2) != NIL);

}

/* xnth - return the nth element of a list */
LVAL xnth()
{
    return (nth(TRUE));
}

/* xnthcdr - return the nth cdr of a list */
LVAL xnthcdr()
{
    return (nth(FALSE));
}

/* nth - internal nth function */
LOCAL LVAL nth(carflag)
  int carflag;
{
    LVAL list,num;
    FIXTYPE n;

    /* get n and the list */
    num = xlgafixnum();
    list = xlgacons();
    xllastarg();

    /* make sure the number isn't negative */
    if ((n = getfixnum(num)) < 0)
	xlfail("bad argument");

    /* find the nth element */
    while (consp(list) && --n >= 0)
	list = cdr(list);

    /* return the list beginning at the nth element */
    return (carflag && consp(list) ? car(list) : list);
}

/* xlength - return the length of a list or string */
LVAL xlength()
{
    FIXTYPE n;
    LVAL arg;

    /* get the list or string */
    arg = xlgetarg();
    xllastarg();

    /* find the length of a list */
    if (listp(arg))
	for (n = 0; consp(arg); n++)
	    arg = cdr(arg);

    /* find the length of a string */
    else if (stringp(arg))
	n = (FIXTYPE)getslength(arg)-1;

    /* find the length of a vector */
    else if (vectorp(arg))
	n = (FIXTYPE)getsize(arg);

    /* otherwise, bad argument type */
    else
	xlerror("bad argument type",arg);

    /* return the length */
    return (cvfixnum(n));
}

/* xmapc - built-in function 'mapc' */
LVAL xmapc()
{
    return (map(TRUE,FALSE));
}

/* xmapcar - built-in function 'mapcar' */
LVAL xmapcar()
{
    return (map(TRUE,TRUE));
}

/* xmapl - built-in function 'mapl' */
LVAL xmapl()
{
    return (map(FALSE,FALSE));
}

/* xmaplist - built-in function 'maplist' */
LVAL xmaplist()
{
    return (map(FALSE,TRUE));
}

/* map - internal mapping function */
LOCAL LVAL map(carflag,valflag)
  int carflag,valflag;
{
    LVAL *newfp,fun,lists,val,last,p,x,y;
    int argc;

    /* protect some pointers */
    xlstkcheck(3);
    xlsave(fun);
    xlsave(lists);
    xlsave(val);

    /* get the function to apply and the first list */
    fun = xlgetarg();
    lists = xlgalist();

    /* initialize the result list */
    val = (valflag ? NIL : lists);

    /* build a list of argument lists */
    for (lists = last = consa(lists); moreargs(); last = cdr(last))
	rplacd(last,cons(xlgalist(),NIL));

    /* loop through each of the argument lists */
    for (;;) {

	/* build an argument list from the sublists */
	newfp = xlsp;
	pusharg(cvfixnum((FIXTYPE)(newfp - xlfp)));
	pusharg(fun);
	pusharg(NIL);
	argc = 0;
	for (x = lists; x && (y = car(x)) && consp(y); x = cdr(x)) {
	    pusharg(carflag ? car(y) : y);
	    rplaca(x,cdr(y));
	    ++argc;
	}

	/* quit if any of the lists were empty */
	if (x) {
	    xlsp = newfp;
	    break;
	}

	/* apply the function to the arguments */
	newfp[2] = cvfixnum((FIXTYPE)argc);
	xlfp = newfp;
	if (valflag) {
	    p = consa(xlapply(argc));
	    if (val) rplacd(last,p);
	    else val = p;
	    last = p;
	}
	else
	    xlapply(argc);
    }

    /* restore the stack */
    xlpopn(3);

    /* return the last test expression value */
    return (val);
}

/* xrplca - replace the car of a list node */
LVAL xrplca()
{
    LVAL list,newcar;

    /* get the list and the new car */
    list = xlgacons();
    newcar = xlgetarg();
    xllastarg();

    /* replace the car */
    rplaca(list,newcar);

    /* return the list node that was modified */
    return (list);
}

/* xrplcd - replace the cdr of a list node */
LVAL xrplcd()
{
    LVAL list,newcdr;

    /* get the list and the new cdr */
    list = xlgacons();
    newcdr = xlgetarg();
    xllastarg();

    /* replace the cdr */
    rplacd(list,newcdr);

    /* return the list node that was modified */
    return (list);
}

/* xnconc - destructively append lists */
LVAL xnconc()
{
    LVAL next,last,val;

    /* initialize */
    val = NIL;
    
    /* concatenate each argument */
    if (moreargs()) {
	while (xlargc > 1) {

	    /* ignore everything except lists */
	    if ((next = nextarg()) && consp(next)) {

		/* concatenate this list to the result list */
		if (val) rplacd(last,next);
		else val = next;

		/* find the end of the list */
		while (consp(cdr(next)))
		    next = cdr(next);
		last = next;
	    }
	}

	/* handle the last argument */
	if (val) rplacd(last,nextarg());
	else val = nextarg();
    }

    /* return the list */
    return (val);
}

/* xdelete - built-in function 'delete' */
LVAL xdelete()
{
    LVAL x,list,fcn,last,val;
    int tresult;

    /* protect some pointers */
    xlsave1(fcn);

    /* get the expression to delete and the list */
    x = xlgetarg();
    list = xlgalist();
    xltest(&fcn,&tresult);

    /* delete leading matches */
    while (consp(list)) {
	if (dotest2(x,car(list),fcn) != tresult)
	    break;
	list = cdr(list);
    }
    val = last = list;

    /* delete embedded matches */
    if (consp(list)) {

	/* skip the first non-matching element */
	list = cdr(list);

	/* look for embedded matches */
	while (consp(list)) {

	    /* check to see if this element should be deleted */
	    if (dotest2(x,car(list),fcn) == tresult)
		rplacd(last,cdr(list));
	    else
		last = list;

	    /* move to the next element */
	    list = cdr(list);
 	}
    }

    /* restore the stack */
    xlpop();

    /* return the updated list */
    return (val);
}

/* xdelif - built-in function 'delete-if' */
LVAL xdelif()
{
    LVAL delif();
    return (delif(TRUE));
}

/* xdelifnot - built-in function 'delete-if-not' */
LVAL xdelifnot()
{
    LVAL delif();
    return (delif(FALSE));
}

/* delif - common routine for 'delete-if' and 'delete-if-not' */
LOCAL LVAL delif(tresult)
  int tresult;
{
    LVAL list,fcn,last,val;

    /* protect some pointers */
    xlsave1(fcn);

    /* get the expression to delete and the list */
    fcn = xlgetarg();
    list = xlgalist();
    xllastarg();

    /* delete leading matches */
    while (consp(list)) {
	if (dotest1(car(list),fcn) != tresult)
	    break;
	list = cdr(list);
    }
    val = last = list;

    /* delete embedded matches */
    if (consp(list)) {

	/* skip the first non-matching element */
	list = cdr(list);

	/* look for embedded matches */
	while (consp(list)) {

	    /* check to see if this element should be deleted */
	    if (dotest1(car(list),fcn) == tresult)
		rplacd(last,cdr(list));
	    else
		last = list;

	    /* move to the next element */
	    list = cdr(list);
 	}
    }

    /* restore the stack */
    xlpop();

    /* return the updated list */
    return (val);
}

/* xsort - built-in function 'sort' */
LVAL xsort()
{
    LVAL sortlist();
    LVAL list,fcn;

    /* protect some pointers */
    xlstkcheck(2);
    xlsave(list);
    xlsave(fcn);

    /* get the list to sort and the comparison function */
    list = xlgalist();
    fcn = xlgetarg();
    xllastarg();

    /* sort the list */
    list = sortlist(list,fcn);

    /* restore the stack and return the sorted list */
    xlpopn(2);
    return (list);
}

/*
    This sorting algorithm is based on a Modula-2 sort written by
    Richie Bielak and published in the February 1988 issue of
    "Computer Language" magazine in a letter to the editor.
*/

/* sortlist - sort a list using quicksort */
LOCAL LVAL sortlist(list,fcn)
  LVAL list,fcn;
{
    LVAL gluelists();
    LVAL smaller,pivot,larger;
    
    /* protect some pointers */
    xlstkcheck(3);
    xlsave(smaller);
    xlsave(pivot);
    xlsave(larger);
    
    /* lists with zero or one element are already sorted */
    if (consp(list) && consp(cdr(list))) {
	pivot = list; list = cdr(list);
	splitlist(pivot,list,&smaller,&larger,fcn);
	smaller = sortlist(smaller,fcn);
	larger = sortlist(larger,fcn);
	list = gluelists(smaller,pivot,larger);
    }

    /* cleanup the stack and return the sorted list */
    xlpopn(3);
    return (list);
}

/* splitlist - split the list around the pivot */
LOCAL splitlist(pivot,list,psmaller,plarger,fcn)
  LVAL pivot,list,*psmaller,*plarger,fcn;
{
    LVAL next;
    
    /* initialize the result lists */
    *psmaller = *plarger = NIL;
    
    /* split the list */
    for (; consp(list); list = next) {
	next = cdr(list);
	if (dotest2(car(list),car(pivot),fcn)) {
	    rplacd(list,*psmaller);
	    *psmaller = list;
	}
	else {
	    rplacd(list,*plarger);
	    *plarger = list;
	}
    }
}

/* gluelists - glue the smaller and larger lists with the pivot */
LOCAL LVAL gluelists(smaller,pivot,larger)
  LVAL smaller,pivot,larger;
{
    LVAL last;
    
    /* larger always goes after the pivot */
    rplacd(pivot,larger);

    /* if the smaller list is empty, we're done */
    if (null(smaller))
	return (pivot);

    /* append the smaller to the front of the resulting list */
    for (last = smaller; consp(cdr(last)); last = cdr(last))
	;
    rplacd(last,pivot);
    return (smaller);
}
