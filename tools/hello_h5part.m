#!/usr/bin/env octave-qf

% Usage (octave-qf wrapper should be on the PATH):
% > ./hello_h5part.m test_data/all.h5part

fn  = argv(){1}; % file name
% fn  = 'test_data/all.h5part';
D   = load('-hdf5', fn);

% ts: timestep; f: field (x, y, ...)
gs = @(ts)    getfield(D, sprintf('Step_%d', ts));
gf = @(ts, f) getfield(gs(ts), f);

ts = 0; xx0 = gf(ts, 'x'); yy0 = gf(ts, 'y'); zz0 = gf(ts, 'z');
ts = 1; xx1 = gf(ts, 'x'); yy1 = gf(ts, 'y'); zz1 = gf(ts, 'z');

dlmwrite(stdout(), [xx1', yy1', zz1'], ' ');
