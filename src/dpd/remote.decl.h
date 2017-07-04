// see the vanilla version of this code for details about how this class
// operates
namespace dpd {
int ncells;
l::rnd::d::KISS *local_trunk;

/* allocated inside init1 */
l::rnd::d::KISS *interrank_trunks[26];
bool interrank_masks[26];

MPI_Comm cart;
MPI_Request sendreq[26 * 2], recvreq[26], sendcellsreq[26], recvcellsreq[26],
    sendcountreq[26], recvcountreq[26];
int recv_tags[26], recv_counts[26];
bool firstpost;
int dstranks[26];

// zero-copy allocation for acquiring the message offsets in the gpu send
// buffer
int *required_send_bag_size, *required_send_bag_size_host;

// plain copy of the offsets for the cpu (i speculate that reading multiple
// times the zero-copy entries is slower)
int nsendreq;
cudaEvent_t evfillall, evuploaded, evdownloaded;

/** were static in pack() **/
int cellpackstarts[27];
k_halo::CellPackSOA cellpacks[26];
int *srccells0[26], *dstcells0[26];
int *srccells1[26], *dstcells1[26];
/***************************/
}
