#!/usr/bin/env python
import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.image as mpimg
from matplotlib.offsetbox import OffsetImage, AnnotationBbox

def cpi_vs_line_size(results):
    # plots graphs of cpi against line size for the different generators
    generators = results['Memory_Generator'].unique()
    line_sizes = sorted(results['Line_Size'].unique())
    icon = mpimg.imread("graphs/bow.png")

    plt.figure(figsize= (10, 6), facecolor='#ffe6f0')
    
    for generator in generators:
        set = results[results['Memory_Generator'] == generator]
        plt.plot(set['Line_Size'], set['CPI'], marker = 'x', label = generator, color = '#E75480')

        for x, y in zip(set['Line_Size'], set['CPI']):
            # plt.text(x, y, '🎀', fontsize=14, ha='center', va='center')
            im = OffsetImage(icon, zoom=0.5)
            ab = AnnotationBbox(im, (x, y), frameon=False, box_alignment=(0.5, 0.5))
            plt.gca().add_artist(ab)



        plt.title('CPI vs L1 Line Size')
        plt.xlabel('L1 Line Size (Bytes)')
        plt.ylabel('CPI')
        plt.legend()

        plt.grid(True)
        plt.xticks(line_sizes)

        # plt.show()

        path = f"graphs/cpi_linesize_{generator}.png"
        plt.savefig(path) 
        plt.close()







def main():
    file = r"simulation_results.csv"

    results = pd.read_csv(file)

    cpi_vs_line_size(results)



if __name__ == '__main__':
    main()

