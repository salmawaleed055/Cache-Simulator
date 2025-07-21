#!/usr/bin/env python
import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.image as mpimg
from matplotlib.offsetbox import OffsetImage, AnnotationBbox
import os
import glob

def cpi_vs_line_size(results, output_folder, filename="default"):
    # plots graphs of cpi against line size for the different generators
    generators = results['Memory_Generator'].unique()
    line_sizes = sorted(results['Line_Size'].unique())
    # icon = mpimg.imread("graphs/bow.png")

    os.makedirs(output_folder, exist_ok=True)

    for generator in generators:
        # plt.figure(figsize= (10, 6), facecolor='#ffe6f0')
        set = results[results['Memory_Generator'] == generator]
        # plt.plot(set['Line_Size'], set['CPI'], marker = 'x', label = generator, color = '#E75480')
        plt.plot(set['Line_Size'], set['CPI'], marker = 'x', label = generator)


        # for x, y in zip(set['Line_Size'], set['CPI']):
        #     im = OffsetImage(icon, zoom=0.5)
        #     ab = AnnotationBbox(im, (x, y), frameon=False, box_alignment=(0.5, 0.5))
        #     plt.gca().add_artist(ab)

        plt.title('CPI vs L1 Line Size')
        plt.xlabel('L1 Line Size (Bytes)')
        plt.ylabel('CPI')
        plt.legend()

        plt.grid(True)
        plt.xticks(line_sizes)

        # plt.show()

        if filename == "default":
            path = os.path.join(output_folder, f"cpi_linesize_{generator}.png")
        else:
            path = os.path.join(output_folder, f"{filename}_{generator}.png")

        plt.savefig(path) 
        plt.close()


def average_cpi_vs_line_size(folder):
    # read all csv files
    all_files = glob.glob(os.path.join(folder, "simulation_results*.csv"))

    data_list = [pd.read_csv(file) for file in all_files]
    combined = pd.concat(data_list)

    # Group by generator and line size, then average CPI
    averaged = combined.groupby(['Memory_Generator', 'Line_Size'], as_index=False).agg({'CPI': 'mean'})

    return averaged


def main():
    try:
        file = r"results/simulation_results5.csv"

        results = pd.read_csv(file)

        cpi_vs_line_size(results, "graphs/data_5")

        # folder = "results"
        # averaged = average_cpi_vs_line_size(folder)
        # cpi_vs_line_size(averaged, "graphs/report_graphs", "average")

    except Exception as e:
        print(f"Error: {e}")


if __name__ == '__main__':
    main()

