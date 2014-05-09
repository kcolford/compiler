/* This is the optimizer.

Copyright (C) 2014 Kieran Colford

This file is part of Compiler.

Compiler is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

Compiler is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with Compiler; see the file COPYING.  If not see
<http://www.gnu.org/licenses/>. */

#include "config.h"

#include "ast.h"
#include "ast_util.h"
#include "compiler.h"
#include "lib.h"
#include "parse.h"

#include <assert.h>

#define FOLD_INTEGER_UNI(OP) do {		\
    if (s->ops[0]->type == integer_type)	\
      {						\
	long long a = s->ops[0]->op.integer.i;	\
	AST_FREE (s);				\
	s = make_integer (OP a);		\
      }						\
  } while (0)

/* Fold up expressions involving constant integer expressions into a
   single constant integer. */
#define FOLD_INTEGER_BIN(OP) do {		\
    if (s->ops[0]->type == integer_type &&	\
	s->ops[1]->type == integer_type)	\
      {						\
	long long l = s->ops[0]->op.integer.i;	\
	long long r = s->ops[1]->op.integer.i;	\
	AST_FREE (s);				\
	s = make_integer (l OP r);		\
      }						\
  } while (0)

/* Convience macro for declaring how to fold up each binary
   operator. */
#define FOLD_INT_BIN(CASE, OP)			\
  case CASE:					\
  FOLD_INTEGER_BIN (OP);			\
  break

static void
optimizer_r (struct ast **ss)
{
  assert (ss != NULL);
#define s (*ss)
  if (s == NULL)
    return;
  optimizer_r (&s->next);
  switch (s->type)
    {
      /* Fold up repetitive allocations into one allocation. */
    case variable_type:
      if (s->next != NULL)
	{
	  int folded = 0;
	  switch (s->next->type)
	    {
	    case variable_type:
	      if (s->next->op.variable.type == NULL
		  && s->next->op.variable.name == NULL)
		{
		  s->op.variable.alloc += s->next->op.variable.alloc;
		  folded = 1;
		}
	      break;

	    case function_call_type:
	      if (STREQ (s->next->ops[0]->loc->base, "__builtin_alloca")
		  && s->ops[1]->type == integer_type)
		{
		  s->op.variable.alloc += s->next->ops[1]->op.integer.i;
		  folded = 1;
		}
	      break;
	    }
	  if (folded)
	    {
	      struct ast *t = s->next;
	      s->next = t->next;
	      t->next = NULL;
	      AST_FREE (t);
	    }
	}
      break;

      /* Fold up predictable if-statements. */
    case cond_type:
      optimizer_r (&s->ops[0]);
      if (s->ops[0]->type == integer_type)
	{
	  struct ast *t = NULL;
	  if (s->ops[0]->op.integer.i)
	    {
	      t = make_jump (xstrdup (s->op.cond.name));
	      t->next = s->next;
	      SWAP (t, s);
	      t->next = NULL;
	    }
	  else
	    {
	      t = s;
	      s = s->next;
	      t->next = NULL;
	    }
	  AST_FREE (t);
	}
      break;

      /* Fold up constant expressions. */
    case binary_type:
      optimizer_r (&s->ops[0]);
      optimizer_r (&s->ops[1]);
      if (optimize > 0)
	{
	  switch (s->op.binary.op)
	    {
	      FOLD_INT_BIN ('+', +);
	      FOLD_INT_BIN ('-', -);
	      FOLD_INT_BIN ('*', *);
	      FOLD_INT_BIN ('/', /);
	      FOLD_INT_BIN ('%', %);
	      FOLD_INT_BIN ('>', >);
	      FOLD_INT_BIN ('<', <);
	      FOLD_INT_BIN (GE, >=);
	      FOLD_INT_BIN (LE, <=);
	      FOLD_INT_BIN (EQ, ==);
	      FOLD_INT_BIN (NE, !=);
	      FOLD_INT_BIN (RS, >>);
	      FOLD_INT_BIN (LS, <<);
	    }
	}
      break;

      /* Fold up constant expressions. */
    case unary_type:
      optimizer_r (&s->ops[0]);
      if (optimize > 0)
	{
	  switch (s->op.unary.op)
	    {
	    case '-':
	      FOLD_INTEGER_UNI (-);
	      break;
	    }
	}
      break;

    default:
      ;
      int j;
      for (j = 0; j < s->num_ops; j++)
	optimizer_r (&s->ops[j]);
    }
#undef s
}

int
optimizer (struct ast **ss)
{
  optimizer_r (ss);
  return 0;
}
