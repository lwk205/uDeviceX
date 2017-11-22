namespace exch {
namespace flu {

void compute_map(const int *start, const int *count, /**/ Pack *p);
void download_cell_starts(/**/ Pack *p);

void pack(const Cloud *cloud, /**/ Pack *p);
void download_data(Pack *p);



void unpack(Unpack *u);

} // flu
} // exch
