namespace x {
void ini(/*io*/ basetags::TagGen *g) {
    cnt = -1; /* TODO: */
    ini_ticketcom(&tc);
    ini_ticketr(&tr);
    ini_tickettags(g, &tt);
    ini_ticketpack(&tp);
    ini_ticketpinned(&ti);
    Dalloc(&buf, MAX_PART_NUM);
    Palloc0(&buf_pi, MAX_PART_NUM);
    rex::ini();
}
}
