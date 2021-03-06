/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "database.h"

bool_t
xdr_message (XDR *xdrs, message *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->content, 2000,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_rpc_args (XDR *xdrs, rpc_args *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->action, 20,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->id))
		 return FALSE;
	 if (!xdr_message (xdrs, &objp->message))
		 return FALSE;
	return TRUE;
}
