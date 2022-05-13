
package edu.rochester.urmc.cbim.dediscover.simulation;

import java.awt.Image;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.event.ActionEvent;
import java.io.Serializable;

import javax.swing.AbstractAction;
import javax.swing.ImageIcon;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;

import org.jfree.chart.ChartPanel;

import edu.rochester.urmc.cbim.dediscover.plotting.DatasetDisplayConfig;


public class ChartMenu {
    private static final String EXPORT_PLOT_COMMAND = "Export Plot...";
    private static final String COPY_PLOT_COMMAND = "Copy";

    static class DefaultPlotPropertyAction extends AbstractAction {

        private DatasetDisplayConfig config;
        private ChartPanel chartPanel;

        public DefaultPlotPropertyAction(DatasetDisplayConfig config,
                                         ChartPanel chartPanel) {
            super("Default Setting", null);
            this.config = config;
            this.chartPanel = chartPanel;
        }

        /*
         * (non-Javadoc)
         *
         * @see
         * java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent
         * )
         */
        @Override
        public void actionPerformed(ActionEvent e) {
            config.clearProperties();
        }
    }

    /**
     * export plot using FreeHEP instead of JFreeChart export
     */
    static class ExportPlotAction extends AbstractAction implements
            Serializable {
        private static final long serialVersionUID = -6192247710058061325L;
        private ChartPanel chartPanel;
        private String defaultFile;

        public ExportPlotAction(String text, ImageIcon icon,
                                ChartPanel chartPanel, String defaultFile) {
            super(text, icon);
            this.chartPanel = chartPanel;
            this.defaultFile = defaultFile;
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            //ExportPlotPanel.ExportViaDialog(chartPanel, defaultFile);
            chartPanel.updateUI();
        }
    }

    /**
     * copy plot to system clip board
     */
    static class CopyPlotAction extends AbstractAction implements Serializable,
            ClipboardOwner {
        private static final long serialVersionUID = 1599988705772403022L;
        private ChartPanel chartPanel;
        private final DataFlavor[] transferDataFlavors = {DataFlavor.imageFlavor};

        public CopyPlotAction(String text, ImageIcon icon, ChartPanel chartPanel) {
            super(text, icon);
            this.chartPanel = chartPanel;
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            Clipboard clipboard = chartPanel.getToolkit().getSystemClipboard();
            // for now, use the height and width of this panel.
            final Image image = chartPanel.getChart().createBufferedImage(
                    chartPanel.getWidth(), chartPanel.getHeight());
            Transferable transferable = new Transferable() {
                public Object getTransferData(DataFlavor flavor) {
                    if (isDataFlavorSupported(flavor)) {
                        return image;
                    }
                    return null;
                }

                public DataFlavor[] getTransferDataFlavors() {
                    return transferDataFlavors;
                }

                public boolean isDataFlavorSupported(DataFlavor flavor) {
                    return DataFlavor.imageFlavor.equals(flavor);
                }
            };
            clipboard.setContents(transferable, this);
        }

        @Override
        public void lostOwnership(Clipboard clipboard, Transferable contents) {
        }
    }

    /**
     * create new JFreeChart Pop-up menu based on new menu items and replace
     * menu items
     *
     *            JFreeChart popupMenu
     */
    public static void adjustNewPopupMenu(final ChartPanel chartPanel,
                                          final DatasetDisplayConfig config) {
        JPopupMenu popupMenu = chartPanel.getPopupMenu();
        // add new Copy PLOT MenuItem
        JMenuItem copyPlotItem = new JMenuItem(new CopyPlotAction(
                COPY_PLOT_COMMAND, null, chartPanel));
        addMenuItem(popupMenu, copyPlotItem);
        // replace "Save as..." MenuItem
        JMenuItem exportPlotItem = new JMenuItem(new ExportPlotAction(
                EXPORT_PLOT_COMMAND, null, chartPanel, "export"));
        popupMenu.remove(2);
        popupMenu.add(exportPlotItem, 2);
        popupMenu.add(new JMenuItem(new DefaultPlotPropertyAction(config,
                chartPanel)));
    }

    /**
     * add menu item into menu, if same menu item name, replace instead of add
     * in order to avoid adding too many same items
     *
     *            popupMenu to be added
     * @param menuItem
     *            menu item
     */
    private static void addMenuItem(JPopupMenu popupMenu, JMenuItem menuItem) {
        String name = menuItem.getText();
        int count = popupMenu.getComponentCount();
        for (int i = 0; i < count; i++) {
            if (popupMenu.getComponent(i) instanceof JMenuItem) {
                JMenuItem item = (JMenuItem) popupMenu.getComponent(i);
                if (item.getText().equals(name)) {
                    popupMenu.remove(i);
                    popupMenu.add(menuItem, i);
                } else {
                    popupMenu.add(menuItem);
                }
            }
        }
    }

}
