#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE    255

//===============================================//
//                                               //
//        Define representation of row           //
//                                               //
//===============================================//
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

const uint32_t ID_SIZE          = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE    = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE       = size_of_attribute(Row, email);
const uint32_t ID_OFFSET        = 0;
const uint32_t USERNAME_OFFSET  = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET     = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE         = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

//
//  Serializes the row pointer to a location in memory
//
void serialize_row(Row* source, void* destination)
{
  memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
  memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
  memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_OFFSET);
}

//
//  Deserializes the objects in memory to a row pointer object
//
void deserialize_row(void* source, Row* destination)
{
  memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
  memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
  memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

//===============================================//
//                                               //
//        Define representation of Table         //
//                                               //
//===============================================//
#define TABLE_MAX_PAGES 100

const uint32_t PAGE_SIZE      = 4096;
const uint32_t ROWS_PER_PAGE  = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

struct Table_t
{

}

//
//  Structure defining the row for inserting
//  the id of the row, the username, and email.
//
struct Row_t
{
  uint32_t  id;
  char      username[COLUMN_USERNAME_SIZE];
  char      email[COLUMN_EMAIL_SIZE];
};
typedef struct Row_t Row;


//
//  Structure to define the pointer and information for
//  the input buffer (the user's input).
//
struct InputBuffer_t
{
  char*   buffer;
  size_t  buffer_length;
  ssize_t input_length;
};
typedef struct InputBuffer_t InputBuffer;

//
//  Enum defining the various statement types
//
enum StatementType_t
{
  STATEMENT_INSERT,
  STATEMENT_SELECT,
};
typedef enum StatementType_t StatementType;

//
//  Structure defining a Statement with its respective type
//
struct Statement_t
{
  StatementType type;
  Row           row_to_insert; //only used by insert statement
};
typedef struct Statement_t Statement;

//
//  Enum defining the meta command statuses.
//
enum MetaCommandResult_t
{
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
};
typedef enum MetaCommandResult_t MetaCommandResult;

//
//  Enum defining the prepare statement statuses
//
enum PrepareResult_t
{
  PREPARE_SUCCESS,
  PREPARE_UNRECOGNIZED_STATEMENT,
  PREPARE_SYNTAX_ERROR
};
typedef enum PrepareResult_t PrepareResult;

//
//  Assigns the appropriate types to the statement pointer,
//  then returns if it was successful or not.
//
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement)
{

  //  Compares the first 6 bytes of the buffer for insert
  if (strncmp(input_buffer->buffer, "insert", 6) == 0)
  {
    statement->type   = STATEMENT_INSERT;
    int args_assigned = sscanf(
      input_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id),
      statement->row_to_insert.username, statement->row_to_insert.gmail));

    if (args_assigned < 3)
    {
      return PREPARE_SYNTAX_ERROR;
    }
    return PREPARE_SUCCESS;
  }

  if(strcmp(input_buffer->buffer, "select") == 0)
  {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED_STATEMENT;

}

//
//  Processes the meta commands
//
MetaCommandResult do_meta_command(InputBuffer* input_buffer)
{
  if(strcmp(input_buffer->buffer, ".exit") == 0)
  {
    exit(EXIT_SUCCESS);
  }
  else
  {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

//
//  Returns an instance of the input buffer
//
InputBuffer* new_input_buffer()
{
  InputBuffer* input_buffer   = malloc(sizeof(InputBuffer));
  input_buffer->buffer        = NULL;
  input_buffer->buffer_length = NULL;
  input_buffer->input_length  = NULL;

  return input_buffer;
}

//
//  Prints the default prompt for the user
//
void print_prompt()
{
  printf("db >");
}

//
//
//
void execute_statement(Statement* statement)
{
  switch(statement->type)
  {
    case (STATEMENT_INSERT):
      printf("This is where we would do an insert.\n");
      break;
    case (STATEMENT_SELECT):
      printf("This is where we would do a select.\n");
      break;
  }
}
//
//  Reads the input from the user
//
void read_input(InputBuffer* input_buffer)
{
  ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

  if (bytes_read <= 0)
  {
    printf("Error reading input\n");
    exit(EXIT_FAILURE);
  }

  //  Ignore trailing newline
  input_buffer->input_length           = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1] = 0;
}

//
//  Deallocates the buffer for the input, then frees
//  the input buffer pointer itself.
//
void close_input_buffer(InputBuffer* input_buffer)
{
  free(input_buffer->buffer);
  free(input_buffer);
}
int main(int argc, char* argv[])
{
  InputBuffer* input_buffer = new_input_buffer();

  while(true)
  {
    print_prompt();
    read_input(input_buffer);

    if (input_buffer->buffer[0] == '.')
    {
      switch (do_meta_command(input_buffer))
      {
        case (META_COMMAND_SUCCESS):
          continue;
        case (META_COMMAND_UNRECOGNIZED_COMMAND):
          printf("Unrecognized command '%s'.\n", input_buffer->buffer);
      }
    }

    Statement statement;
    switch (prepare_statement(input_buffer, &statement))
    {
      case (PREPARE_SUCCESS):
        break;
      case (PREPARE_UNRECOGNIZED_STATEMENT):
        printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
        continue;
    }

    execute_statement(&statement);
    printf("Executed.\n");
  }
}
