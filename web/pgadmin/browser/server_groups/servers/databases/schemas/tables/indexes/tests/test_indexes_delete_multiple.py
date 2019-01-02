##########################################################################
#
# pgAdmin 4 - PostgreSQL Tools
#
# Copyright (C) 2013 - 2019, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
##########################################################################

import uuid
import json

from pgadmin.browser.server_groups.servers.databases.schemas.tables.column. \
    tests import utils as columns_utils
from pgadmin.browser.server_groups.servers.databases.schemas.tables.tests \
    import utils as tables_utils
from pgadmin.browser.server_groups.servers.databases.schemas.tests import \
    utils as schema_utils
from pgadmin.browser.server_groups.servers.databases.tests import utils as \
    database_utils
from pgadmin.utils.route import BaseTestGenerator
from regression import parent_node_dict
from regression.python_test_utils import test_utils as utils
from . import utils as indexes_utils


class IndexesDeleteMultipleTestCase(BaseTestGenerator):
    """This class will delete the existing index of column."""
    scenarios = [
        ('Delete index Node URL', dict(url='/browser/index/obj/'))
    ]

    def setUp(self):
        self.db_name = parent_node_dict["database"][-1]["db_name"]
        schema_info = parent_node_dict["schema"][-1]
        self.server_id = schema_info["server_id"]
        self.db_id = schema_info["db_id"]
        db_con = database_utils.connect_database(self, utils.SERVER_GROUP,
                                                 self.server_id, self.db_id)
        if not db_con['data']["connected"]:
            raise Exception("Could not connect to database to add a table.")
        self.schema_id = schema_info["schema_id"]
        self.schema_name = schema_info["schema_name"]
        schema_response = schema_utils.verify_schemas(self.server,
                                                      self.db_name,
                                                      self.schema_name)
        if not schema_response:
            raise Exception("Could not find the schema to add a table.")
        self.table_name = "table_column_%s" % (str(uuid.uuid4())[1:8])
        self.table_id = tables_utils.create_table(self.server, self.db_name,
                                                  self.schema_name,
                                                  self.table_name)
        self.column_name = "test_column_delete_%s" % (str(uuid.uuid4())[1:8])
        self.column_id = columns_utils.create_column(self.server,
                                                     self.db_name,
                                                     self.schema_name,
                                                     self.table_name,
                                                     self.column_name)
        self.index_name = "test_index_delete_%s" % (str(uuid.uuid4())[1:8])
        self.index_name_1 = "test_index_delete_%s" % (str(uuid.uuid4())[1:8])
        self.index_ids = [indexes_utils.create_index(self.server, self.db_name,
                                                     self.schema_name,
                                                     self.table_name,
                                                     self.index_name,
                                                     self.column_name),
                          indexes_utils.create_index(self.server, self.db_name,
                                                     self.schema_name,
                                                     self.table_name,
                                                     self.index_name_1,
                                                     self.column_name)
                          ]

    def runTest(self):
        """This function will delete index of existing column."""
        index_response = indexes_utils.verify_index(self.server, self.db_name,
                                                    self.index_name)
        if not index_response:
            raise Exception("Could not find the index to delete.")

        index_response = indexes_utils.verify_index(self.server, self.db_name,
                                                    self.index_name_1)
        if not index_response:
            raise Exception("Could not find the index to delete.")

        data = {'ids': self.index_ids}
        response = self.tester.delete(self.url + str(utils.SERVER_GROUP) +
                                      '/' + str(self.server_id) + '/' +
                                      str(self.db_id) + '/' +
                                      str(self.schema_id) + '/' +
                                      str(self.table_id) + '/',
                                      data=json.dumps(data),
                                      content_type='html/json',
                                      follow_redirects=True)
        self.assertEquals(response.status_code, 200)

    def tearDown(self):
        # Disconnect the database
        database_utils.disconnect_database(self, self.server_id, self.db_id)
