# -*- coding: utf-8 -*-

#	SAGA Modules plugin for Quantum GIS
#
#	plugin.py (C) Camilo Polymeris
#	
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
# 
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#       
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#   MA 02110-1301, USA.

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from qgis.core import *

import os
import processingplugin.processing as processing
import processingplugin.processing.parameters as parameters
import otbApplication as otb

class OTBPlugin():
    def __init__(self, iface):
        print "OTBPlugin::__init__"
        self._modules = None
        self.iface = iface
        self.window = iface.mainWindow()
        pass
        
    def initGui(self):
        print "OTBPlugin::initGui"
        processing.framework.registerModuleProvider(self)
        
    def unload(self):
        print "OTBPlugin::unload"
        pass # TODO : unload the modules
    
    def modules(self):
        print "OTBPlugin::modules"
        if self._modules is None:
            apps = otb.Registry.GetAvailableApplications()
            print "available apps : " + str(apps)
            self._modules = set()
            for app in apps:
                self._modules.add(OTBModule(app))
        return self._modules

class OTBModule(processing.Module):
    def __init__(self, modulename):
        print "OTBModule __init__ " + modulename
        self._app = otb.Registry.CreateApplication(modulename)
        processing.Module.__init__(self,
            self._app.GetName(),
            self._app.GetDescription())
        
        self._parameters = []
        
        self._instance = OTBModuleInstance(self)
        
        self.layerRegistry = QgsMapLayerRegistry.instance()
        
        for p in self._app.GetParametersKeys():
            #print p
            self.addParameter(p)

    def addParameter(self, otbParamKey):
        print "addParameter " + otbParamKey
        otbToQGisParam = {
            otb.ParameterType_Empty:               parameters.BooleanParameter,
            otb.ParameterType_Int:                 parameters.NumericParameter,
            otb.ParameterType_Float:               parameters.NumericParameter,
            otb.ParameterType_String:              parameters.StringParameter,
            otb.ParameterType_Filename:            parameters.PathParameter,
            otb.ParameterType_Directory:           parameters.PathParameter,
            otb.ParameterType_Choice:              parameters.ChoiceParameter,
            otb.ParameterType_InputImage:          parameters.RasterLayerParameter,
            otb.ParameterType_InputComplexImage:   parameters.RasterLayerParameter,
            otb.ParameterType_InputVectorData:     parameters.VectorLayerParameter,
            otb.ParameterType_OutputImage:         parameters.PathParameter,
            otb.ParameterType_OutputVectorData:    parameters.PathParameter,
            otb.ParameterType_Radius:              parameters.NumericParameter
        }
        
        name = otbParamKey
        descr = self._app.GetParameterName(otbParamKey)
        typ = self._app.GetParameterType(otbParamKey)
        role = parameters.Parameter.Role.input # TODO
        mandatory = self._app.IsMandatory(otbParamKey)

        try:
            qgisParamClass = otbToQGisParam[typ]

            if not qgisParamClass: # We are ignoring certain types of parameters
                print "Ignoring type " + str(typ)
                return

            qgisParam = qgisParamClass(name, descr, role=role)
            
            qgisParam._app = self._app
            qgisParam._key = otbParamKey

            if qgisParamClass == parameters.NumericParameter:
                # TODO (handle no value case) qgisParam.setDefaultValue(self._app.GetParameterFloat(otbParamKey))
                qgisParam.setDefaultValue(0)

            elif qgisParamClass == parameters.ChoiceParameter:
                qgisParam.setChoices(self._app.GetChoiceNames(otbParamKey))
                qgisParam.setDefaultValue(self._app.GetParameterInt(otbParamKey))

            elif (qgisParamClass == parameters.VectorLayerParameter or
                qgisParamClass == parameters.RasterLayerParameter):
                if role == parameters.Parameter.Role.output:
                    self._instance.outLayer.append(qgisParam)
                else:
                    self._instance.inLayer.append(qgisParam)
                
                # force update of layer
                self.onParameterChanged(qgisParam, None)

            # TODO validator            

            qgisParam.setMandatory(mandatory)
            self._parameters.append(qgisParam)

            # register callback to instance for parameter
            QObject.connect(self._instance,
                            self._instance.valueChangedSignal(qgisParam),
                            lambda x: self.onParameterChanged(qgisParam, x))

        except KeyError:
            print otbParamKey + " is of unhandled parameter type."
            pass


    def parameters(self):
        return self._parameters

    def tags(self):
        return processing.Module.tags(self) | set([processing.Tag('otb')])
    
    def onParameterChanged(self, qgisParam, value):
        pc = qgisParam.__class__
        app = qgisParam._app
        key = qgisParam._key

        print "onParameterChanged " + key + " " + str(value)
        
        if pc == parameters.VectorLayerParameter:
                qgisParam.layer = value
                if value is None: return
                dpUri = str(param.layer.dataProvider().dataSourceUri())
                dpDescription = param.layer.dataProvider().description()              
                isLocal = dpDescription.startsWith('GDAL provider')
                if isLocal:
                  print "Setting " + dpUri + " to param VectorLayerParameter " + key
                  app.SetParameterString(key, dpUri)
        elif pc == parameters.RasterLayerParameter:
                print "onParameterChanged " + key + " " + str(value)
                if value is None: return
                qgisParam.layer = value
                dpUri = str(qgisParam.layer.dataProvider().dataSourceUri())
                dpDescription = qgisParam.layer.dataProvider().description()              
                isLocal = dpDescription.startsWith('OGR data provider')
                if isLocal:
                  print "Setting " + dpUri + " to param RasterLayerParameter " + key
                  app.SetParameterString(key, dpUri)
#        elif pc == RangeParameter:
#            low, high = value
#            sagaParam.asRange().Set_Range(low, high)
#        elif pc == GridSystemParameter:
#            cellsize = value.cellsize
#            xMin, xMax = value.xRange
#            yMin, yMax = value.yRange
#            self.module.Get_System().Assign(cellsize, xMin, yMin,
#                xMax, yMax)
        elif pc == StringParameter:
            # convert QString to CSG_String
            print "Setting " + str(value) + " to param StringParameter " + key
            app.SetParameterString(key, value)
        elif pc == NumericParameter:
            if app.GetParameterType(key) == ParameterType_Int:
              print "Setting " + str(value) + " to param intParameter " + key
              app.SetParameterInt(key, int(value))
            else:
              print "Setting " + str(value) + " to param floatParameter " + key
              app.SetParameterFloat(key, float(value))
        elif pc == BooleanParameter:
            if value :
              print "Enable " + key
              app.EnableParameter(key)
            else :
              print "Disable " + key
              app.DisableParameter(key)
            


class OTBModuleInstance(processing.ModuleInstance):
    def __init__(self, module):
        print "OTBModuleInstance  " + module.name()
        processing.ModuleInstance.__init__(self, module)
        QObject.connect(
            self, self.valueChangedSignal(self.stateParameter),
            self.stateParameterValueChanged)
        self.inLayer = list()
        self.outLayer = list()
        
    def stateParameterValueChanged(self, state):
        """ Only reacts to start running state, ignore others.
        """
        print "stateParameterValueChanged " + str(state)
        sm = self.module().module # the SAGA module
        if state != StateParameter.State.running:
            return
        modName = self.module().name()
 
        print "Executing " + modName
        app = self.module()._app
        app.ExecuteAndWriteOutput()

        if sm.Execute() != 0:
            self.setFeedback("SAGA Module execution suceeded.")
            
            # umm- what if there is no iface?
            iface = self.module().iface
            # now import output layers
            for param in self.outLayer:
                basename = "otb-qgis-%s-%s-%s" % (self.module().name(), param._key, id(param))
                pc = param.__class__
                if pc == parameters.VectorLayerParameter:
                    iface.addVectorLayer(app.GetParameterString(param._key), basename, "ogr")
                elif pc == parameters.RasterLayerParameter:
                    iface.addRasterLayer(app.GetParameterString(param._key), basename)
        else:
            self.setFeedback("Module execution failed.")
        self.setState(StateParameter.State.stopped)






