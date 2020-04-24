#include "DarknetLoader.h"
#include "DarknetDispatcher.h"
#include "GlobalVariables.h"


DarknetLoader::DarknetLoader(QString modelName) :
    isLoaded(false),
    model(modelName)
{

}

DarknetLoader::~DarknetLoader()
{
    unload();
}

void DarknetLoader::run()
{
    if (isLoaded) {
        QApplication::restoreOverrideCursor();
        emit loadFailed("There is already a loaded model.");
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    cuda_set_device(0); // Set proper index in future?

    QString dataDir = GlobalVariables::dataDir + "/" + model + "/";
    QString cfg = model + ".cfg";
    QString weights = model + ".weights";
    QString data = model + ".data";
    QString names = model + ".names";

    QByteArray cfgstr = (dataDir + cfg).toLocal8Bit();
    QByteArray weightstr = (dataDir + weights).toLocal8Bit();
    QByteArray datastr = (dataDir + data).toLocal8Bit();
    QByteArray namesstr = (dataDir + names).toLocal8Bit();

    char* cfgfile = cfgstr.data();
    char* weightfile = weightstr.data();
    char* datafile = datastr.data();
    char* namesfile = namesstr.data();

    if (!QFile(cfgfile).exists()) {
        QApplication::restoreOverrideCursor();
        emit loadFailed("Config file not found!");
        return;
    }
    if (!QFile(weightfile).exists()) {
        QApplication::restoreOverrideCursor();
        emit loadFailed("Weight file not found!");
        return;
    }
    if (!QFile(datafile).exists()) {
        QApplication::restoreOverrideCursor();
        emit loadFailed("Data file not found!");
        return;
    }
    if (!QFile(namesfile).exists()) {
        QApplication::restoreOverrideCursor();
        emit loadFailed("Names file file not found!");
        return;
    }

    darknet_list *options = read_data_cfg(cfgfile);
    char name_option[] = "names";
    char *name_list = option_find_str(options, &name_option[0], namesfile);
    m_names = get_labels(name_list);
    net = parse_network_cfg_custom(cfgfile, 1, 1);
    load_weights(&net, weightfile);
    fuse_conv_batchnorm(net);
    calculate_binary_weights(net);

    char class_option[] = "classes";
    m_classes = option_find_int(options, &class_option[0], 20);

    DarknetInformation info = {m_classes, m_names, &net};
    isLoaded = true;
    emit networkLoaded(info);
    QApplication::restoreOverrideCursor();
}

void DarknetLoader::unload()
{
    if (isLoaded) {
        free_network(net);
        isLoaded = false;
    }
}
