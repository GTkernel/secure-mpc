import matplotlib.pyplot as plt
import numpy as np
#from pprint import pprint
#import matplotlib.backends.backend_pdf
import sys
import argparse
import enum

sentinalStr = "SeNtInAl"

class TAG_TYPE(enum.Enum):
   UNDEFINED = 0
   TIMER = 1
   COUNTER = 2
   HISTOGRAM = 2

class tag:
    def __init__(self):
        self.tagType = TAG_TYPE.UNDEFINED
        self.name = ""



def cleanse(ticTocFilePath, tagWhitelist):
    tags = []
    with open(ticTocFilePath, "r") as f:
        lines = f.readlines()

    with open(ticTocFilePath, "w") as f:
        for line in lines:
            if len(line) > len(sentinalStr) and line[:len(sentinalStr)] == sentinalStr:
                f.write(line)
                tokens = line.split(',')

                whitelisted = tagWhitelist == None or tokens[3] in tagWhitelist

                newTag = True
                for t in tags:
                    if t.name == tokens[3]:
                        newTag = False
                        break

                if newTag and whitelisted:
                    print("found new tag: " + tokens[3])
                    t = tag()
                    if tokens[1] == "timer":
                        t.tagType = TAG_TYPE.TIMER
                    elif tokens[1] == "counter":
                        t.tagType = TAG_TYPE.COUNTER
                    elif tokens[1] == "histogram":
                        t.tagType = TAG_TYPE.HISTOGRAM
                    t.name = tokens[3]
                    tags.append(t)
    return tags



def main(ticTocFilePath, graphPath, tags, xlabel):
    data = np.genfromtxt(ticTocFilePath, dtype=None,
            delimiter=',', encoding=None, filling_values="0", names="sentinal, func, timer, value, unit")

    fig_id = 0

    for t in tags:
        tagSpecificValues = []
        for row in data:
            if (row[3] == t.name):
                tagSpecificValues.append(row[4]);

        #print(tagSpecificValues)

        if t.tagType == TAG_TYPE.TIMER:
            print("plotting timer tag: " + t.name)
            # box plot
            plt.figure(fig_id)
            fig_id += 1
            myplot = plt.boxplot(tagSpecificValues)
            plt.xlabel('')
            plt.ylabel('seconds')
            plt.title(prefix + " " + t.name + " box plot (" + str(len(tagSpecificValues))+" runs)")
            #plt.show()
            plt.savefig(graphPath + "/" + prefix + "_" + t.name + "_box.pdf")

            # xy plot
            plt.figure(fig_id)
            fig_id += 1
            plt.plot(np.linspace(0, len(tagSpecificValues), len(tagSpecificValues), endpoint=False), tagSpecificValues)
            plt.xlabel(xlabel)
            plt.ylabel('seconds')
            plt.title(prefix + " " + t.name + " xy plot")
            #plt.show()
            plt.savefig(graphPath + "/" + prefix + "_" + t.name + "_xy.pdf")

        elif t.tagType == TAG_TYPE.COUNTER:
            print("plotting counter tag: " + t.name)
            # xy plot
            plt.figure(fig_id)
            fig_id += 1
            plt.plot(np.linspace(0, len(tagSpecificValues), len(tagSpecificValues), endpoint=False), tagSpecificValues)
            plt.xlabel(xlabel)
            plt.ylabel('count')
            plt.title(prefix + " " + t.name + " xy plot")
            #plt.show()
            plt.savefig(graphPath + "/" + prefix + "_" + t.name + "_xy.pdf")
            # total

        elif t.tagType == TAG_TYPE.HISTOGRAM:
            print("plotting histogram tag: " + t.name)
            # total


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="TIC TOC plotter")
    parser.add_argument('--csvlog',
                    required=True,
                    help=("Path to program log containing TIC TOC macros"))
    parser.add_argument('--graphpath',
                    required=True,
                    help=("Path to store graphs"))
    parser.add_argument('--prefix',
                    required=False,
                    help=("Prefix to append to all stored files and plot titles"))
    parser.add_argument('--xlabel',
                    required=False,
                    default="index",
                    help=("Label to use for plot x axis"))
    parser.add_argument('--only-tags',
                    required=False,
                    nargs="+",
                    help=("Only plot specific (space seperated) tags in csv log files. Default plots all tags."))
    options = parser.parse_args()

    prefix = ""
    if options.prefix != None:
        prefix = options.prefix

    try:
        ticTocFilePath = options.csvlog
        with open(ticTocFilePath, "r") as f:
            print("found file " + ticTocFilePath)
    except FileNotFoundError:
        print("file not found " + ticTocFilePath)
        exit

    if options.only_tags != None:
        print("Only plotting specific tags")
        print(options.only_tags)

    tags = cleanse(ticTocFilePath, options.only_tags)
    main(ticTocFilePath, options.graphpath, tags, options.xlabel)

