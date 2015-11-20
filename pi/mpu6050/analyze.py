import sys
import json
import numpy
import pprint

def main():
    samples = json.load(open(sys.argv[1]))
    res = {}
    for key in samples[0].keys():
        if key == 'timestamp' or not isinstance(samples[0][key], list):
            continue
        for i, name in enumerate("xyz"):
            a = numpy.array([s[key][i] for s in samples])
            res[key + "_" + name] = dict(
                mean=numpy.mean(a),
                std=numpy.std(a),
                var=numpy.var(a),
            )
    print(json.dumps(res, indent=4))


if __name__ == '__main__':
    main()
