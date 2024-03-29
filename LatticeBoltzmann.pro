# -------------------------------------------------
# Project created by QtCreator 2010-06-06T19:47:47
# -------------------------------------------------

QT += opengl \
    svg \
    xml \
    script \
    widgets
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += printsupport
    QT += concurrent
}
TARGET = LatticeBoltzmann
TEMPLATE = app
INCLUDEPATH += "view/util"

include(qwt/qwt.prf)

SOURCES += \
    lbmain.cpp \
    view/lbmainwindow.cpp \
    view/lbwidget.cpp \
    view/info.cpp \
    model/latticeboltzmann/boundary/wallcell.cpp \
    model/latticeboltzmann/boundary/sourcecell.cpp \
    model/latticeboltzmann/boundary/opencell.cpp \
    model/listener/listenerdata.cpp \
    model/latticeboltzmann/lbgrid.cpp \
    view/painter/painter.cpp \
    view/painter/camera.cpp \
    model/inputoutput/loaderfromimage.cpp \
    model/inputoutput/loaderfromtext.cpp \
    view/view.cpp \
    view/util/scrolldecorator.cpp \
    model/latticeboltzmann/extra/velocityfield.cpp \
    model/inputoutput/iterationreporter.cpp \
    model/inputoutput/simulationreporter.cpp \
    model/latticeboltzmann/boundary/zouheboundary.cpp \
    view/util/executor.cpp \
    model/latticeboltzmann/multi/mccell.cpp \
    model/latticeboltzmann/sccell.cpp \
    model/latticeboltzmann/force/gravity.cpp \
    model/latticeboltzmann/force/multiphase.cpp \
    model/latticeboltzmann/lbutil.cpp \
    model/latticeboltzmann/gridconfig.cpp \
    view/colors.cpp \
    model/latticeboltzmann/moving/movingcell.cpp \
    model/latticeboltzmann/moving/movingparticle.cpp \
    model/math/vector3i.cpp \
    model/latticeboltzmann/moving/particlemanager.cpp \
    model/latticeboltzmann/gridsimulation.cpp \
    model/latticeboltzmann/moving/gridchange.cpp \
    view/chart.cpp \
    view/util/xypoint.cpp \
    model/latticeboltzmann/boundary/equilibriumboundary.cpp \
    model/latticeboltzmann/extra/particle.cpp \
    model/latticeboltzmann/physical/parameters.cpp \
    view/view2.cpp \
    view/util/improvedlineedit.cpp \
    model/latticeboltzmann/force/multicomponent.cpp \
    view/about.cpp \
    view/particles.cpp \
    model/latticeboltzmann/extra/startingparticle.cpp \
    model/latticeboltzmann/boundary/fixedboundary.cpp \
    model/latticeboltzmann/boundary/extrapolationboundary.cpp \
    model/latticeboltzmann/moving/movingwall.cpp \
    model/latticeboltzmann/force/forcelist.cpp \
    model/latticeboltzmann/porouscell.cpp \
    model/latticeboltzmann/multi/rkcell.cpp \
    model/latticeboltzmann/shallow/shallowcell.cpp \
    view/util/singleton.cpp \
    view/util/historylineedit.cpp \
    model/latticeboltzmann/multi/fecell.cpp \
    model/latticeboltzmann/nullcell.cpp \
    model/latticeboltzmann/boundary/pointboundary.cpp \
    model/latticeboltzmann/thermal/passivescalar.cpp \
    model/latticeboltzmann/thermal/thermalwall.cpp \
    model/inputoutput/matlabreporter.cpp \
    view/results.cpp \
    model/latticeboltzmann/boundary/dragwallcell.cpp \
    model/latticeboltzmann/boundary/partialslipboundary.cpp \
    model/math/myvector3d.cpp \
    model/math/myquaternion.cpp \
    model/latticeboltzmann/basecell.cpp \
    view/changes.cpp \
    view/codeeditor.cpp \
    model/latticeboltzmann/deposition/depositioncell.cpp \
    model/latticeboltzmann/deposition/depositionwall.cpp \
    view/interpolation.cpp \
    model/latticeboltzmann/interpolation/interpolationcell.cpp \
    #model/latticeboltzmann/opencl/opencldevice.cpp \
    #model/latticeboltzmann/opencl/openclsingleton.cpp \
    #model/latticeboltzmann/opencl/openclplatform.cpp \
    #view/opencl.cpp \
    #model/latticeboltzmann/opencl/multilatticeboltzmannopencl.cpp \
    #model/latticeboltzmann/opencl/allocation.cpp \
    #view/command/setopenclcommand.cpp \
    model/util/shared.cpp \
    view/animation.cpp \
    view/painter/paintercell.cpp \
    #model/latticeboltzmann/opencl/lbocl.cpp \
    model/latticeboltzmann/extra/initialparticle.cpp \
    model/latticeboltzmann/passivescalar/passivescalarcell.cpp \
    model/latticeboltzmann/passivescalar/passivescalarsingleton.cpp \
    model/latticeboltzmann/passivescalar/configuration.cpp \
    view/parameters2.cpp \
    model/latticeboltzmann/boundary/reflectwithfactorcell.cpp \
    model/inputoutput/vtkreporter.cpp \
    model/latticeboltzmann/streamline/streamlinecurve.cpp \
    model/latticeboltzmann/streamline/streamlinegenerator.cpp \
    view/painterconfig.cpp \
    model/latticeboltzmann/streamline/streamlinealgorithm.cpp \
    model/latticeboltzmann/streamline/euler.cpp \
    model/latticeboltzmann/streamline/velocityevaluator.cpp \
    model/latticeboltzmann/streamline/velocitylinearinterpolator.cpp \
    model/latticeboltzmann/streamline/nearestpointvelocityevaluator.cpp \
    model/latticeboltzmann/streamline/seedgenerator.cpp \
    model/latticeboltzmann/streamline/nextcellevalutor.cpp \
    view/painter/isolineseeder.cpp \
    view/painter/equallyspacedseeder.cpp \
    view/painter/customseeder.cpp \
    model/math/util.cpp \
    view/painter/isosurfacehelper.cpp \
    model/latticeboltzmann/force/passivescalarforce.cpp \
    view/mesher.cpp \
    view/moreinfo.cpp \
    model/latticeboltzmann/passivescalar/momentpropagationcell.cpp \
    view/imageprocessing.cpp \
    view/geometry.cpp \
    model/latticeboltzmann/meltingsolidification/meltingsolidificationcell.cpp \
    model/latticeboltzmann/meltingsolidification/kornerimplementation.cpp \
    model/latticeboltzmann/immersed/immersedboundarycontainer.cpp \
    model/latticeboltzmann/immersed/immerseditem.cpp \
    model/latticeboltzmann/immersed/circleitem.cpp \
    model/latticeboltzmann/immersed/lineitem.cpp \
    model/math/mytwovector3d.cpp \
    view/immersed.cpp \
    model/latticeboltzmann/immersed/rectangleitem.cpp \
    model/latticeboltzmann/immersed/fileitem.cpp
HEADERS += \
    view/lbmainwindow.h \
    view/lbwidget.h \
    view/info.h \
    model/latticeboltzmann/boundary/wallcell.h \
    model/latticeboltzmann/boundary/sourcecell.h \
    model/listener/listenerdata.h \
    model/listener/listener.h \
    model/latticeboltzmann/lbgrid.h \
    view/painter/painter.h \
    view/painter/camera.h \
    model/inputoutput/loaderfromimage.h \
    model/inputoutput/loaderfromtext.h \
    view/view.h \
    view/util/scrolldecorator.h \
    model/latticeboltzmann/extra/velocityfield.h \
    model/inputoutput/iterationreporter.h \
    model/latticeboltzmann/boundary/opencell.h \
    model/inputoutput/simulationreporter.h \
    model/latticeboltzmann/boundary/boundary.h \
    model/latticeboltzmann/boundary/zouheboundary.h \
    view/util/executor.h \
    model/latticeboltzmann/multi/mccell.h \
    model/latticeboltzmann/basecell.h \
    model/latticeboltzmann/sccell.h \
    model/latticeboltzmann/force/force.h \
    model/latticeboltzmann/force/gravity.h \
    model/latticeboltzmann/force/multiphase.h \
    model/latticeboltzmann/lbutil.h \
    model/latticeboltzmann/gridconfig.h \
    view/colors.h \
    model/latticeboltzmann/moving/movingcell.h \
    model/latticeboltzmann/moving/movingparticle.h \
    model/math/vector3i.h \
    model/latticeboltzmann/moving/particlemanager.h \
    model/latticeboltzmann/gridsimulation.h \
    model/latticeboltzmann/moving/gridchange.h \
    view/chart.h \
    view/util/xypoint.h \
    model/latticeboltzmann/boundary/equilibriumboundary.h \
    model/latticeboltzmann/extra/particle.h \
    model/latticeboltzmann/physical/parameters.h \
    view/view2.h \
    view/util/improvedlineedit.h \
    model/latticeboltzmann/force/multicomponent.h \
    view/about.h \
    view/particles.h \
    model/latticeboltzmann/extra/startingparticle.h \
    model/latticeboltzmann/boundary/fixedboundary.h \
    model/latticeboltzmann/boundary/extrapolationboundary.h \
    model/latticeboltzmann/moving/somethingmoving.h \
    model/latticeboltzmann/moving/movingwall.h \
    model/latticeboltzmann/force/forcelist.h \
    model/latticeboltzmann/porouscell.h \
    model/latticeboltzmann/multi/rkcell.h \
    model/latticeboltzmann/shallow/shallowcell.h \
    view/util/singleton.h \
    view/util/historylineedit.h \
    model/latticeboltzmann/multi/fecell.h \
    model/latticeboltzmann/nullcell.h \
    model/latticeboltzmann/boundary/pointboundary.h \
    model/latticeboltzmann/thermal/passivescalar.h \
    model/latticeboltzmann/thermal/thermalwall.h \
    model/inputoutput/matlabreporter.h \
    view/results.h \
    model/latticeboltzmann/boundary/dragwallcell.h \
    model/latticeboltzmann/boundary/partialslipboundary.h \
    model/math/myvector3d.h \
    model/math/myquaternion.h \
    #model/latticeboltzmann/opencl/lbocl.h \
    view/changes.h \
    view/codeeditor.h \
    model/latticeboltzmann/deposition/depositioncell.h \
    model/latticeboltzmann/deposition/depositionwall.h \
    view/interpolation.h \
    model/latticeboltzmann/interpolation/interpolationcell.h \
    #model/latticeboltzmann/opencl/opencldevice.h \
    #model/latticeboltzmann/opencl/openclsingleton.h \
    #model/latticeboltzmann/opencl/openclplatform.h \
    #view/opencl.h \
    #model/latticeboltzmann/opencl/multilatticeboltzmannopencl.h \
    #model/latticeboltzmann/opencl/allocation.h \
    #view/command/setopenclcommand.h \
    model/util/shared.h \
    view/animation.h \
    view/painter/paintercell.h \
    model/latticeboltzmann/extra/initialparticle.h \
    model/latticeboltzmann/passivescalar/passivescalarcell.h \
    model/latticeboltzmann/passivescalar/passivescalarsingleton.h \
    model/latticeboltzmann/passivescalar/configuration.h \
    view/parameters2.h \
    model/latticeboltzmann/boundary/reflectwithfactorcell.h \
    model/inputoutput/vtkreporter.h \
    model/latticeboltzmann/streamline/streamlinecurve.h \
    model/latticeboltzmann/streamline/streamlinegenerator.h \
    view/painterconfig.h \
    model/latticeboltzmann/streamline/streamlinealgorithm.h \
    model/latticeboltzmann/streamline/euler.h \
    model/latticeboltzmann/streamline/velocityevaluator.h \
    model/latticeboltzmann/streamline/velocitylinearinterpolator.h \
    model/latticeboltzmann/streamline/nearestpointvelocityevaluator.h \
    model/latticeboltzmann/streamline/seedgenerator.h \
    model/latticeboltzmann/streamline/nextcellevalutor.h \
    view/painter/isolineseeder.h \
    view/painter/equallyspacedseeder.h \
    view/painter/customseeder.h \
    model/math/util.h \
    view/painter/isosurfacehelper.h \
    model/latticeboltzmann/force/passivescalarforce.h \
    view/mesher.h \
    view/moreinfo.h \
    model/latticeboltzmann/passivescalar/momentpropagationcell.h \
    view/imageprocessing.h \
    view/geometry.h \
    model/latticeboltzmann/meltingsolidification/meltingsolidificationcell.h \
    model/latticeboltzmann/meltingsolidification/kornerimplementation.h \
    model/latticeboltzmann/immersed/immersedboundarycontainer.h \
    model/latticeboltzmann/immersed/immerseditem.h \
    model/latticeboltzmann/immersed/circleitem.h \
    model/latticeboltzmann/immersed/lineitem.h \
    model/math/mytwovector3d.h \
    view/immersed.h \
    model/latticeboltzmann/immersed/rectangleitem.h \
    model/latticeboltzmann/immersed/fileitem.h
FORMS += \
    view/lbmainwindow.ui \
    view/info.ui \
    view/view.ui \
    view/colors.ui \
    view/chart.ui \
    view/view2.ui \
    view/about.ui \
    view/particles.ui \
    view/results.ui \
    view/changes.ui \
    view/codeeditor.ui \
    view/interpolation.ui \
    #view/opencl.ui \
    view/animation.ui \
    view/parameters2.ui \
    view/painterconfig.ui \
    view/mesher.ui \
    view/moreinfo.ui \
    view/imageprocessing.ui \
    view/geometry.ui \
    view/immersed.ui
