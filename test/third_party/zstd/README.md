Test bench applications from zstd github repo
====================================================================

Following test applications are included:
- `bigdict`             : test for large dictionary sizes
- `decodecorpus`        : tool to generate valid Zstandard frames, for verifying decoder implementations
- `fullbench`           : precisely measure speed for each zstd inner functions
- `fuzzer`              : test tool, to check zstd integrity on target platform
- `invaliddictionaries` : test with invalid dictionaries
- `longmatch`           : test for long matches
- `paramgrill`          : parameter tester for zstd
- `pooltests`           : thread pool tester
- `zstreamtest`         : fuzzer test tool for zstd streaming API


Excluded tests
====================================================================
- `legacy` : Test tool to test decoding of legacy zstd frames
			 Legacy zstd frame formats are not supported
