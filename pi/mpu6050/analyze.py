import sys
import json
import numpy
import pprint

def main():
    samples = json.load(sys.stdin)
    res = {}
    for key in samples[0].keys():
        if key == 'timestamp':
            continue
        for i, name in enumerate("xyz"):
            a = numpy.array([s[key][i] for s in samples])
            res[key + "_" + name] = dict(
                mean=numpy.mean(a),
                std=numpy.std(a),
                var=numpy.var(a),
            )
    pprint.pprint(res)


if __name__ == '__main__':
    main()
