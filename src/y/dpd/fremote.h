namespace dpd {
void fremote(int n, SendHalo* sendhalos[], RecvHalo* recvhalos[],
             l::rnd::d::KISS* interrank_trunks[], bool interrank_masks[], /**/ Force *ff) {
    static BipsBatch::BatchInfo infos[26];

    for (int i = 0; i < 26; ++i) {
        int dx = (i     + 2) % 3 - 1;
        int dy = (i / 3 + 2) % 3 - 1;
        int dz = (i / 9 + 2) % 3 - 1;

        int m0 = 0 == dx;
        int m1 = 0 == dy;
        int m2 = 0 == dz;

        BipsBatch::BatchInfo entry = {
            (float *)sendhalos[i]->dbuf->D,
            (float2 *)recvhalos[i]->dbuf->D,
            interrank_trunks[i]->get_float(),
            sendhalos[i]->dbuf->S,
            recvhalos[i]->dbuf->S,
            interrank_masks[i],
            recvhalos[i]->dcellstarts->D,
            sendhalos[i]->scattered_entries->D,
            dx,
            dy,
            dz,
            1 + m0 * (XS - 1),
            1 + m1 * (YS - 1),
            1 + m2 * (ZS - 1),
            (BipsBatch::HaloType)(abs(dx) + abs(dy) + abs(dz))};

        infos[i] = entry;
    }

    BipsBatch::interactions(infos, (float *)ff);
}
} /* namespace dpd */
