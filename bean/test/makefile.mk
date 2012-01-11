#**************************************************************
#  
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#  
#**************************************************************



PRJ := ..
PRJNAME := bean
TARGET := test
PACKAGE = test

.INCLUDE: settings.mk

#----- compile .java files -----------------------------------------

JARFILES = officebean.jar ridl.jar unoil.jar jurt.jar juh.jar java_uno.jar 
JAVAFILES       = Test.java
JAVACLASSFILES	= $(foreach,i,$(JAVAFILES) $(CLASSDIR)$/$(PACKAGE)$/$(i:b).class)

#----- make a jar from compiled files ------------------------------

MAXLINELENGTH = 100000

#JARCLASSDIRS    = $(PACKAGE)
#JARTARGET       = $(TARGET).jar
#JARCOMPRESS 	= TRUE

# --- Parameters for the test --------------------------------------

# test base is java complex
#CT_TESTBASE = -TestBase java_complex

# test looks something like the.full.package.TestName
#CT_TEST     = -o $(PACKAGE:s\$/\.\).$(JAVAFILES:b)

# start the runner application
#CT_APP      = org.openoffice.Runner

#CT_NOOFFICE = -NoOffice


OFFICE_CLASSPATH_TMP:=$(foreach,i,$(JARFILES) $(office)$/program$/classes$/$(i)$(PATH_SEPERATOR))

OFFICE_CLASSPATH=$(OFFICE_CLASSPATH_TMP:t"")$(PATH_SEPERATOR)$(CLASSDIR)

OOOBEAN_OPTIONS=-Dcom.sun.star.officebean.Options=-norestore 


.INCLUDE: target.mk

ALLTAR : RUNINSTRUCTIONS

# --- Targets ------------------------------------------------------

#The OOoBean uses the classpath to find the office installation.
#Therefore we must use the jar files from the office.
RUN:
    java -cp $(OFFICE_CLASSPATH) $(OOOBEAN_OPTIONS) $(PACKAGE).Test
run: RUN

rund:
    java -Xdebug -Xrunjdwp:transport=dt_socket,server=y,address=8100 -cp $(OFFICE_CLASSPATH) $(OOOBEAN_OPTIONS) $(PACKAGE).Test



RUNINSTRUCTIONS : 
    @echo .
    @echo ###########################   N O T E  ######################################
    @echo . 
    @echo "To run the test you have to provide the office location."
    @echo Example:
    @echo dmake run office="d:/myOffice"
    @echo .
  
