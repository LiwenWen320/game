#!/usr/bin/python

import networkx, math, igraph
import matplotlib.pyplot as plt
from pymongo import MongoClient


def draw_graph(graph, cooper_data, reputation, file):
    g = networkx.read_graphml(graph)
    nodes = [int(n.lstrip('n')) for n in g.nodes()]
    print len(nodes), len(cooper_data), len(reputation)
    pos = networkx.spring_layout(g)
    node_color = ['r' if cooper_data[n] > 0 else 'b' for n in nodes]
    node_size= [math.log(reputation[n]+1) for n in nodes]
    networkx.draw(g, pos,  node_size=node_size, node_color=node_color, linewidths=0.1, width=0.1, with_labels=False)
    plt.show()

def draw(graph, step, strats, reputs, gfile):
    g = igraph.Graph.Read_GraphML(graph)
    nodes = [int(n['id'].lstrip('n')) for n in g.vs()]
    layout = g.layout_kamada_kawai()
    visual_style = {}
    visual_style["vertex_size"] = [math.log(reputs[n]+1)*3 + 10 for n in nodes]
    visual_style["vertex_color"] = ['red' if strats[n] > 0 else 'blue' for n in nodes]
    visual_style["layout"] = layout
    visual_style["bbox"] = (800, 800)
    visual_style["margin"] = 20
    igraph.plot(g, gfile, **visual_style)
    pass

def mongo_data(db_name, step):
    client = MongoClient('mongodb://localhost:27017/')
    db = client[db_name]
    simus_c = db.simulations
    reputs_c = db.reputations
    strats_c = db.strategies

    if db_name == "seg_pgg_tw_deg":
        record = simus_c.find({"b":{"$gt": 3.19, "$lt": 3.21}, "w":{"$gt": 0.79, "$lt": 0.81}, "ps": 0.01})[0]
    elif db_name == "seg_pgg_tpr_deg":
        record = simus_c.find({"b":{"$gt": 3.19, "$lt": 3.21}, "w":{"$gt": 0.19, "$lt": 0.21}, "ps": 0.01})[0]
    elif db_name == "seg_pdg_tw_deg":
        record = simus_c.find({"b":{"$gt": 1.19, "$lt": 1.21}, "w":{"$gt": 0.79, "$lt": 0.81}, "ps": 0.01})[0]
    elif db_name == "seg_pdg_tpr_deg":
        record = simus_c.find({"b":{"$gt": 1.19, "$lt": 1.21}, "w":{"$gt": 0.19, "$lt": 0.21}, "ps": 0.01})[0]
    timestamp = record['timestamp']
    reput = reputs_c.find({"timestamp": timestamp, "vector_name": str(step)})
    strat = strats_c.find({"timestamp": timestamp, "vector_name": str(step)})

    reputs = []
    strats = []
    for i in xrange(0, len(reput[0].keys())-3):
        reputs.append(reput[0][str(i)])
        strats.append(strat[0][str(i)])

    return reputs, strats

def pgg():
    step = 1
    reput, strat = mongo_data("seg_pgg_tw_deg", step);
    draw("graph_1407196754784_1.gxl", step, strat, reput, "graph_pgg_tw_1.pdf")

    step = 1000
    reput, strat = mongo_data("seg_pgg_tw_deg", step);
    draw("graph_1407196754784_1000.gxl", step, strat, reput, "graph_pgg_tw_1000.pdf")

    step = 10000
    reput, strat = mongo_data("seg_pgg_tw_deg", step);
    draw("graph_1407196754784_10000.gxl", step, strat, reput, "graph_pgg_tw_10000.pdf")

    step = 20000
    reput, strat = mongo_data("seg_pgg_tw_deg", step);
    draw("graph_1407196754784_20000.gxl", step, strat, reput, "graph_pgg_tw_20000.pdf")

    step = 1
    reput, strat = mongo_data("seg_pgg_tpr_deg", step);
    draw("graph_1407196759717_1.gxl", step, strat, reput, "graph_pgg_tpr_1.pdf")

    step = 1000
    reput, strat = mongo_data("seg_pgg_tpr_deg", step);
    draw("graph_1407196759717_1000.gxl", step, strat, reput, "graph_pgg_tpr_1000.pdf")

    step = 10000
    reput, strat = mongo_data("seg_pgg_tpr_deg", step);
    draw("graph_1407196759717_10000.gxl", step, strat, reput, "graph_pgg_tpr_10000.pdf")

    step = 20000
    reput, strat = mongo_data("seg_pgg_tpr_deg", step);
    draw("graph_1407196759717_20000.gxl", step, strat, reput, "graph_pgg_tpr_20000.pdf")

def pdg():
    step = 1
    reput, strat = mongo_data("seg_pdg_tw_deg", step);
    draw("graph_1407199059557_1.gxl", step, strat, reput, "graph_pdg_tw_1.pdf")

    step = 1000
    reput, strat = mongo_data("seg_pdg_tw_deg", step);
    draw("graph_1407199059557_1000.gxl", step, strat, reput, "graph_pdg_tw_1000.pdf")

    step = 10000
    reput, strat = mongo_data("seg_pdg_tw_deg", step);
    draw("graph_1407199059557_10000.gxl", step, strat, reput, "graph_pdg_tw_10000.pdf")

    step = 20000
    reput, strat = mongo_data("seg_pdg_tw_deg", step);
    draw("graph_1407199059557_20000.gxl", step, strat, reput, "graph_pdg_tw_20000.pdf")

    step = 1
    reput, strat = mongo_data("seg_pdg_tpr_deg", step);
    draw("graph_1407199062706_1.gxl", step, strat, reput, "graph_pdg_tpr_1.pdf")

    step = 1000
    reput, strat = mongo_data("seg_pdg_tpr_deg", step);
    draw("graph_1407199062706_1000.gxl", step, strat, reput, "graph_pdg_tpr_1000.pdf")

    step = 10000
    reput, strat = mongo_data("seg_pdg_tpr_deg", step);
    draw("graph_1407199062706_10000.gxl", step, strat, reput, "graph_pdg_tpr_10000.pdf")

    step = 20000
    reput, strat = mongo_data("seg_pdg_tpr_deg", step);
    draw("graph_1407199062706_20000.gxl", step, strat, reput, "graph_pdg_tpr_20000.pdf")

def main():
    
    pdg()

    pass

if __name__ == '__main__':
    main()
