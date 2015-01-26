
int peg_peg(peg *p) {
  save(p);
  peg_SPC(p);
  if (!peg_rule(p)) return fail(p);
  while(peg_rule(p)) ;
  return success(p);
}  

int peg_rule(peg *p) {
  save(p);
  if (!peg_rulename(p)) return fail(p);
  if (!peg_ASSIGN(p)) return fail(p);
  if (!peg_expr(p))) return fail(p);
  if (!peg_ENDRULE(p)) return fail(p);
  return success(p);
}

int peg_rulename(peg *p) {
  save(p);
  if (!peg_id(p)) return fail(p);
  return success(p);
}

int peg_id(peg *p) {
  save(p);
  if (!peg__range(p,"A-Za-z")) return fail(p);
  while (peg__range(p,"0-9A-Za-z"));
  return success(p);
}

int peg_expr(peg *p) {
  save(p);
  if (!peg_term(p)) return fail(p);
  while (!peg_expr_1(p)) ;
  return success(p);
}

int peg_expr_1(peg *p) {
  save(p);
  if (!peg_ALT(p)) return fail(p);
  if (!peg_term(p)) return fail(p);
  return success(p);
}

int peg_term(peg *p) {
  save(p);
  if (!peg_term_1(p)) return fail(p);
  while(peg_term_1(p)) ;
  return success(p);
}

int peg_term_1(peg *p) {
  save(p);
  if (!peg_seq(p)) return fail(p);
  peg_SPC(p);
  return success(p);
}

int peg_seq(peg *p) {
  save(p);
  do {
    if (!p_seq_1(p)) break;
    if (!p_atom(p)) break;
    return success(p);
  }
  back(p);
  
  do {
    if (!p_atom(p)) break;
    p_seq_2(p));
    return success(p);
  }
  back(p);
  
  return fail(p);
}





